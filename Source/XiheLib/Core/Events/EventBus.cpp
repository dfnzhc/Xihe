/**
 * @File EventBus.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/27
 * @Brief This file is part of Xihe.
 */

#include "EventBus.hpp"

#include <algorithm>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include <stop_token>
#include <atomic>
#include <queue>

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wextra-semi-stmt")
XIHE_CLANG_DISABLE_WARNING("-Wunused-parameter")
XIHE_CLANG_DISABLE_WARNING("-Wshadow-field-in-constructor")
XIHE_CLANG_DISABLE_WARNING("-Wzero-as-null-pointer-constant")
#include <eventpp/utilities/argumentadapter.h>
#include <eventpp/utilities/conditionalfunctor.h>
#include <eventpp/utilities/orderedqueuelist.h>
#include <eventpp/utilities/anydata.h>
#include <eventpp/eventdispatcher.h>
#include <eventpp/eventqueue.h>
XIHE_POP_WARNING

using namespace xihe;

namespace {
struct EventWrap
{
    EventPtr event;
    std::type_index id;
    EventPriority priority;
    TimePoint timestamp;

    EventWrap(EventPtr evt) :
        event(std::move(evt))
      , id(event->typeId())
      , priority(event->getPriority())
      , timestamp(event->getTimestamp())
    {
    }
};

struct EventCompare
{
    template <typename T>
    bool operator()(const T& a, const T& b) const
    {
        // 高优先级的事件应该先处理
        const auto& wrapA = a.template getArgument<0>();
        const auto& wrapB = b.template getArgument<0>();
        return wrapA.priority > wrapB.priority;
    }
};

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wunneeded-member-function")

// 同步分发策略
struct EventPolicy
{
    static std::type_index getEvent(const EventWrap& wrap)
    {
        return wrap.id;
    }
};

// 异步队列策略
struct EventQueuePolicy
{
    template <typename Item>
    using QueueList = eventpp::OrderedQueueList<Item, EventCompare>;

    static std::type_index getEvent(const EventWrap& wrap)
    {
        return wrap.id;
    }
};

XIHE_POP_WARNING
} // namespace 

class EventBus::Impl
{
public:
    using DispatcherType = eventpp::EventDispatcher<std::type_index, void(const EventWrap&), EventPolicy>;
    using QueueType      = eventpp::EventQueue<std::type_index, void(const EventWrap&), EventQueuePolicy>;

    using DispatcherHandle = DispatcherType::Handle;
    using QueueHandle      = QueueType::Handle;

    using HandleRecordDispatcher = std::unordered_map<Handle, DispatcherHandle>;
    using HandleRecordQueue      = std::unordered_map<Handle, QueueHandle>;
    using EventRecord            = std::unordered_map<Handle, std::type_index>;

    DispatcherType dispatcher;
    QueueType queue;

    std::shared_mutex dispatcherMutex;
    std::shared_mutex queueMutex;
    std::mutex handleMutex;

    HandleRecordDispatcher dispatcherRecord;
    HandleRecordQueue queueRecord;
    EventRecord eventRecord;

    std::atomic<Handle> nextHandle{1};
    std::jthread queueWorker;
    std::stop_source stopSource;

    std::atomic<u64> dispatchedCount{0};
    std::atomic<u64> queuedCount{0};

    bool removeHandle(Handle handle)
    {
        // Try dispatcher first
        {
            std::scoped_lock lock{handleMutex, dispatcherMutex};
            if (dispatcherRecord.contains(handle) && eventRecord.contains(handle))
            {
                auto dispatcherHandle = dispatcherRecord.extract(handle).mapped();
                auto eventType        = eventRecord.extract(handle).mapped();
                return dispatcher.removeListener(eventType, dispatcherHandle);
            }
        }

        // Try queue if not found in dispatcher
        {
            std::scoped_lock lock{handleMutex, queueMutex};
            if (queueRecord.contains(handle) && eventRecord.contains(handle))
            {
                auto queueHandle = queueRecord.extract(handle).mapped();
                auto eventType   = eventRecord.extract(handle).mapped();
                return queue.removeListener(eventType, queueHandle);
            }
        }

        return false;
    }

    bool isDispatchHandle(Handle handle)
    {
        DispatcherHandle disHandle;
        std::optional<std::type_index> eventId;

        {
            std::scoped_lock lock{handleMutex, dispatcherMutex};
            if (dispatcherRecord.contains(handle) && eventRecord.contains(handle))
            {
                disHandle = dispatcherRecord.at(handle);
                eventId   = eventRecord.at(handle);
            }
        }

        if (disHandle && eventId.has_value())
            return dispatcher.ownsHandle(eventId.value(), disHandle);

        return false;
    }

    bool isQueueHandle(Handle handle)
    {
        QueueHandle queHandle;
        std::optional<std::type_index> eventId;

        {
            std::scoped_lock lock{handleMutex, queueMutex};
            if (queueRecord.contains(handle) && eventRecord.contains(handle))
            {
                queHandle = queueRecord.at(handle);
                eventId   = eventRecord.at(handle);
            }
        }

        if (queHandle && eventId.has_value())
            return queue.ownsHandle(eventId.value(), queHandle);

        return false;
    }

    void queueProcess(std::stop_token stopToken)
    {
        while (true)
        {
            while (!queue.waitFor(std::chrono::milliseconds(20)) && !stopToken.stop_requested());
            if (stopToken.stop_requested())
                break;

            queue.process();
        }
    }

    size_t getSubscriberCount()
    {
        std::shared_lock dispatcherLock(dispatcherMutex);
        std::shared_lock queueLock(queueMutex);
        return dispatcherRecord.size() + queueRecord.size();
    }
};

// ======================================

EventBus::EventBus() :
    _pImpl(std::make_unique<Impl>())
{
    _pImpl->queueWorker = std::jthread(&Impl::queueProcess, _pImpl.get(), _pImpl->stopSource.get_token());
}

EventBus::~EventBus()
{
    if (_pImpl)
    {
        _pImpl->stopSource.request_stop();
        if (_pImpl->queueWorker.joinable())
        {
            _pImpl->queueWorker.join();
        }

        // 处理剩余事件
        _pImpl->queue.process();
    }
}

Handle EventBus::subscribeDirectImpl(std::type_index eventType, GenericEventCallback inCallback) const
{
    auto handle = _pImpl->nextHandle.fetch_add(1);

    auto wrappedCallback = [callback = std::move(inCallback)](const EventWrap& wrap)
    {
        callback(wrap.event);
    };

    // 按照统一的锁顺序获取所有需要的锁
    std::scoped_lock lock{_pImpl->handleMutex, _pImpl->dispatcherMutex};

    auto dispatcherHandle = _pImpl->dispatcher.appendListener(eventType, std::move(wrappedCallback));
    _pImpl->dispatcherRecord.emplace(handle, dispatcherHandle);
    _pImpl->eventRecord.emplace(handle, eventType);

    return handle;
}

Handle EventBus::subscribeQueuedImpl(std::type_index eventType, GenericEventCallback inCallback) const
{
    auto handle = _pImpl->nextHandle.fetch_add(1);

    auto wrappedCallback = [callback = std::move(inCallback)](const EventWrap& wrap)
    {
        callback(wrap.event);
    };

    // 按照统一的锁顺序获取所有需要的锁
    std::scoped_lock lock{_pImpl->handleMutex, _pImpl->queueMutex};

    auto queueHandle = _pImpl->queue.appendListener(eventType, std::move(wrappedCallback));
    _pImpl->queueRecord.emplace(handle, queueHandle);
    _pImpl->eventRecord.emplace(handle, eventType);

    return handle;
}

bool EventBus::unsubscribe(Handle handle) const
{
    return _pImpl->removeHandle(handle);
}

void EventBus::unsubscribeAll() const
{
    std::scoped_lock lock{_pImpl->handleMutex, _pImpl->dispatcherMutex, _pImpl->queueMutex};

    // 移除所有dispatcher监听器
    for (const auto& [handle, dispatcherHandle] : _pImpl->dispatcherRecord)
    {
        auto it = _pImpl->eventRecord.find(handle);
        if (it != _pImpl->eventRecord.end())
        {
            _pImpl->dispatcher.removeListener(it->second, dispatcherHandle);
        }
    }

    // 移除所有queue监听器
    for (const auto& [handle, queueHandle] : _pImpl->queueRecord)
    {
        auto it = _pImpl->eventRecord.find(handle);
        if (it != _pImpl->eventRecord.end())
        {
            _pImpl->queue.removeListener(it->second, queueHandle);
        }
    }

    _pImpl->dispatcherRecord.clear();
    _pImpl->queueRecord.clear();
    _pImpl->eventRecord.clear();
}

void EventBus::dispatch(EventPtr event) const
{
    if (!event || event->isCancelled())
        return;

    std::shared_lock lock(_pImpl->dispatcherMutex);
    EventWrap wrap(event);
    _pImpl->dispatcher.dispatch(wrap.id, wrap);
    _pImpl->dispatchedCount.fetch_add(1);
}

void EventBus::enqueue(EventPtr event, EventPriority priority) const
{
    if (!event || event->isCancelled())
        return;

    event->setPriority(priority);

    std::shared_lock lock(_pImpl->queueMutex);
    EventWrap wrap(event);
    _pImpl->queue.enqueue(wrap.id, wrap);
    _pImpl->queuedCount.fetch_add(1);
}

void EventBus::processBatch(const std::vector<EventPtr>& events)
{
    for (const auto& event : events)
    {
        if (event && !event->isCancelled())
        {
            dispatch(event);
        }
    }
}

void EventBus::processQueue()
{
    _pImpl->queue.process();
}

void EventBus::clearQueue()
{
    _pImpl->queue.clearEvents();
}

u64 EventBus::getDispatchedCount() const
{
    return _pImpl->dispatchedCount.load();
}

u64 EventBus::getQueuedCount() const
{
    return _pImpl->queuedCount.load();
}

size_t EventBus::getSubscriberCount() const
{
    return _pImpl->getSubscriberCount();
}

EventBus::Statistics EventBus::getStatistics() const
{
    Statistics stats;
    stats.dispatchedCount = getDispatchedCount();
    stats.queuedCount     = getQueuedCount();
    stats.subscriberCount = getSubscriberCount();
    return stats;
}
