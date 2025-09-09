/**
 * @File EventBus.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/27
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>

#include "Core/Events/Event.hpp"

namespace xihe {
// 组合过滤器
template <cEventType E>
class CompositeFilter
{
public:
    void addFilter(EventFilter<E> filter)
    {
        _filters.push_back(std::move(filter));
    }

    void clearFilters()
    {
        _filters.clear();
    }

    bool operator()(const E& event) const
    {
        return std::all_of(_filters.begin(), _filters.end(),
                           [&event](const auto& filter)
                           {
                               return filter(event);
                           });
    }

    XIHE_NODISCARD bool empty() const
    {
        return _filters.empty();
    }

private:
    std::vector<EventFilter<E>> _filters;
};

// 预定义过滤器
namespace filters {
    template <cEventType E>
    EventFilter<E> byCategory(EventCategory category)
    {
        return [category](const E& event)
        {
            return IsSet(category, event.getCategory());
        };
    }

    template <cEventType E>
    EventFilter<E> byPriority(EventPriority minPriority)
    {
        return [minPriority](const E& event)
        {
            return event.getPriority() >= minPriority;
        };
    }

    template <cEventType E>
    EventFilter<E> notCancelled()
    {
        return [](const E& event)
        {
            return !event.isCancelled();
        };
    }

    template <cEventType E>
    EventFilter<E> timeRange(TimePoint start, TimePoint end)
    {
        return [start, end](const E& event)
        {
            auto timestamp = event.getTimestamp();
            return timestamp >= start && timestamp <= end;
        };
    }
}

class XIHE_API EventBus
{
public:
    using Handle                          = u64;
    static constexpr Handle InvalidHandle = 0;

    EventBus();
    ~EventBus();

    // 禁止拷贝和移动
    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&)                 = delete;
    EventBus& operator=(EventBus&&)      = delete;

    // -----------------------------
    // 类型安全的订阅接口
    // -----------------------------

    // 同步订阅，立即分发
    template <cEventType E>
    Handle subscribeDirect(EventCallback<E> listener);

    // 异步订阅，队列分发
    template <cEventType E>
    Handle subscribeQueued(EventCallback<E> listener);

    template <cEventType E>
    Handle subscribe(EventCallback<E> listener)
    {
        return subscribeDirect(std::move(listener));
    }

    template <cEventType E>
    Handle subscribeAsync(EventCallback<E> listener)
    {
        return subscribeQueued(std::move(listener));
    }

    template <cEventType E>
    Handle subscribe(EventCallback<E> listener, EventFilter<E> filter)
    {
        return subscribeWithFilter(std::move(listener), std::move(filter));
    }

    template <cEventType E>
    Handle subscribeAsync(EventCallback<E> inListener, EventFilter<E> inFilter)
    {
        // 创建包装回调，在队列处理时应用过滤器
        auto wrappedCallback = [listener = std::move(inListener), filter = std::move(inFilter)](const E& event)
        {
            if (filter(event))
            {
                listener(event);
            }
        };
        return subscribeQueued(std::move(wrappedCallback));
    }

    template <cEventType E>
    Handle subscribeWithFilter(EventCallback<E> inListener, EventFilter<E> inFilter)
    {
        auto filteredListener = [listener = std::move(inListener), filter = std::move(inFilter)](const E& event)
        {
            if (filter(event))
            {
                listener(event);
            }
        };

        return subscribeDirect<E>(std::move(filteredListener));
    }

    // -----------------------------
    // 取消订阅
    // -----------------------------

    bool unsubscribe(Handle handle) const;
    void unsubscribeAll() const;

    // -----------------------------
    // 事件分发
    // -----------------------------

    // 同步分发
    void dispatch(EventPtr event) const;

    // 异步分发
    void dispatchAsync(EventPtr event, EventPriority priority = EventPriority::Normal)
    {
        enqueue(std::move(event), priority);
    }

    // 异步入队
    void enqueue(EventPtr event, EventPriority priority = EventPriority::Normal) const;

    // 批量处理
    void processBatch(const std::vector<EventPtr>& events);

    // -----------------------------
    // 队列管理
    // -----------------------------

    void processQueue();
    void clearQueue();

    // -----------------------------
    // 统计信息
    // -----------------------------

    /**
     * @brief 事件统计信息结构
     */
    struct Statistics
    {
        u64 dispatchedCount  = 0; // 已分发的事件数量
        u64 queuedCount      = 0; // 已排队的事件数量
        Size subscriberCount = 0; // 订阅者数量
    };

    XIHE_NODISCARD u64 getDispatchedCount() const;
    XIHE_NODISCARD u64 getQueuedCount() const;
    XIHE_NODISCARD Size getSubscriberCount() const;
    XIHE_NODISCARD Statistics getStatistics() const;

private:
    class Impl;
    std::unique_ptr<Impl> _pImpl;

    Handle subscribeDirectImpl(std::type_index eventType, GenericEventCallback callback) const;
    Handle subscribeQueuedImpl(std::type_index eventType, GenericEventCallback callback) const;
};

template <cEventType E>
Handle EventBus::subscribeDirect(EventCallback<E> listener)
{
    return subscribeDirectImpl(EventTypeId<E>(), WrapCallback<E>(std::move(listener)));
}

template <cEventType E>
Handle EventBus::subscribeQueued(EventCallback<E> listener)
{
    return subscribeQueuedImpl(EventTypeId<E>(), WrapCallback<E>(std::move(listener)));
}
} // namespace xihe
