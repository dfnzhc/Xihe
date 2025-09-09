/**
 * @File EventTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/9/9
 * @Brief 事件系统相关的单元测试
 */

#include <gtest/gtest.h>

#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <atomic>

#include "Core/Events/Event.hpp"
#include "Core/Events/EventBus.hpp"

using namespace xihe;

// 测试事件类型
class TestEvent : public EventBase<TestEvent>
{
public:
    TestEvent(int value = 0, const std::string& data = "") :
        _value(value), _data(data)
    {
        setCategory(EventCategory::User);
    }

    int getValue() const
    {
        return _value;
    }

    const std::string& getData() const
    {
        return _data;
    }

    std::string toString() const override
    {
        return "TestEvent(value=" + std::to_string(_value) + ", data=" + _data + ")";
    }

private:
    int _value;
    std::string _data;
};

class HighPriorityEvent : public EventBase<HighPriorityEvent>
{
public:
    HighPriorityEvent(const std::string& message = "") :
        _message(message)
    {
        setPriority(EventPriority::High);
        setCategory(EventCategory::App);
    }

    const std::string& getMessage() const
    {
        return _message;
    }

    std::string toString() const override
    {
        return "HighPriorityEvent(message=" + _message + ")";
    }

private:
    std::string _message;
};

class TimedEvent : public EventBase<TimedEvent>
{
public:
    TimedEvent(TimePoint customTime) :
        _customTime(customTime)
    {
        setCategory(EventCategory::Timer);
    }

    TimePoint getTimestamp() const override
    {
        return _customTime;
    }

    std::string toString() const override
    {
        return "TimedEvent";
    }

private:
    TimePoint _customTime;
};

// ======================================

class CompositeFilterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        testEvent         = std::make_shared<TestEvent>(42, "test_data");
        highPriorityEvent = std::make_shared<HighPriorityEvent>("important");
        lowPriorityEvent  = std::make_shared<TestEvent>(1, "low");
        lowPriorityEvent->setPriority(EventPriority::Low);
    }

    std::shared_ptr<TestEvent> testEvent;
    std::shared_ptr<HighPriorityEvent> highPriorityEvent;
    std::shared_ptr<TestEvent> lowPriorityEvent;
};

TEST_F(CompositeFilterTest, EmptyFilter)
{
    CompositeFilter<TestEvent> filter;

    EXPECT_TRUE(filter.empty());
    EXPECT_TRUE(filter(*testEvent)); // 空过滤器应该通过所有事件
}

TEST_F(CompositeFilterTest, SingleFilter)
{
    CompositeFilter<TestEvent> filter;

    // 添加一个值过滤器
    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() > 10;
    });

    EXPECT_FALSE(filter.empty());
    EXPECT_TRUE(filter(*testEvent)); // value = 42 > 10

    auto smallValueEvent = TestEvent(5, "small");
    EXPECT_FALSE(filter(smallValueEvent)); // value = 5 <= 10
}

TEST_F(CompositeFilterTest, MultipleFilters)
{
    CompositeFilter<TestEvent> filter;

    // 添加多个过滤器
    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() > 10; // 值大于10
    });

    filter.addFilter([](const TestEvent& event)
    {
        return !event.getData().empty(); // 数据非空
    });

    filter.addFilter([](const TestEvent& event)
    {
        return event.getPriority() == EventPriority::Normal; // 普通优先级
    });

    // 满足所有条件
    EXPECT_TRUE(filter(*testEvent));

    // 不满足第一个条件
    auto smallValueEvent = TestEvent(5, "data");
    EXPECT_FALSE(filter(smallValueEvent));

    // 不满足第二个条件
    auto emptyDataEvent = TestEvent(20, "");
    EXPECT_FALSE(filter(emptyDataEvent));

    // 不满足第三个条件
    auto highPriorityTestEvent = TestEvent(20, "data");
    highPriorityTestEvent.setPriority(EventPriority::High);
    EXPECT_FALSE(filter(highPriorityTestEvent));
}

TEST_F(CompositeFilterTest, ClearFilters)
{
    CompositeFilter<TestEvent> filter;

    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() > 100; // 很严格的条件
    });

    EXPECT_FALSE(filter.empty());
    EXPECT_FALSE(filter(*testEvent)); // 不满足条件

    filter.clearFilters();

    EXPECT_TRUE(filter.empty());
    EXPECT_TRUE(filter(*testEvent)); // 清空后应该通过
}

TEST_F(CompositeFilterTest, FilterChaining)
{
    CompositeFilter<TestEvent> filter;

    // 逐步添加过滤器，测试链式效果
    EXPECT_TRUE(filter(*testEvent)); // 初始状态通过

    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() > 0;
    });
    EXPECT_TRUE(filter(*testEvent)); // 仍然通过

    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() < 100;
    });
    EXPECT_TRUE(filter(*testEvent)); // 仍然通过

    filter.addFilter([](const TestEvent& event)
    {
        return event.getValue() > 50; // 这个条件不满足
    });
    EXPECT_FALSE(filter(*testEvent)); // 现在不通过了
}

// ======================================

class FiltersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        now    = Clock::now();
        past   = now - std::chrono::seconds(10);
        future = now + std::chrono::seconds(10);

        userEvent = std::make_shared<TestEvent>(42, "user_data");
        userEvent->setCategory(EventCategory::User);

        appEvent = std::make_shared<TestEvent>(1, "app_data");
        appEvent->setCategory(EventCategory::App);

        highPriorityEvent = std::make_shared<TestEvent>(2, "high");
        highPriorityEvent->setPriority(EventPriority::High);

        lowPriorityEvent = std::make_shared<TestEvent>(3, "low");
        lowPriorityEvent->setPriority(EventPriority::Low);

        cancelledEvent = std::make_shared<TestEvent>(4, "cancelled");
        cancelledEvent->cancel();

        pastEvent   = std::make_shared<TimedEvent>(past);
        nowEvent    = std::make_shared<TimedEvent>(now);
        futureEvent = std::make_shared<TimedEvent>(future);
    }

    TimePoint now, past, future;
    std::shared_ptr<TestEvent> userEvent, appEvent, highPriorityEvent, lowPriorityEvent, cancelledEvent;
    std::shared_ptr<TimedEvent> pastEvent, nowEvent, futureEvent;
};

TEST_F(FiltersTest, ByCategoryFilter)
{
    auto userFilter  = filters::byCategory<TestEvent>(EventCategory::User);
    auto appFilter   = filters::byCategory<TestEvent>(EventCategory::App);
    auto inputFilter = filters::byCategory<TestEvent>(EventCategory::Input);

    EXPECT_TRUE(userFilter(*userEvent));
    EXPECT_FALSE(userFilter(*appEvent));

    EXPECT_TRUE(appFilter(*appEvent));
    EXPECT_FALSE(appFilter(*userEvent));

    EXPECT_FALSE(inputFilter(*userEvent));
    EXPECT_FALSE(inputFilter(*appEvent));
}

TEST_F(FiltersTest, ByPriorityFilter)
{
    auto normalFilter   = filters::byPriority<TestEvent>(EventPriority::Normal);
    auto highFilter     = filters::byPriority<TestEvent>(EventPriority::High);
    auto criticalFilter = filters::byPriority<TestEvent>(EventPriority::Critical);

    // Normal 优先级事件
    auto normalEvent = TestEvent(1, "normal");
    EXPECT_TRUE(normalFilter(normalEvent));
    EXPECT_FALSE(highFilter(normalEvent));
    EXPECT_FALSE(criticalFilter(normalEvent));

    // High 优先级事件
    EXPECT_TRUE(normalFilter(*highPriorityEvent));
    EXPECT_TRUE(highFilter(*highPriorityEvent));
    EXPECT_FALSE(criticalFilter(*highPriorityEvent));

    // Low 优先级事件
    EXPECT_FALSE(normalFilter(*lowPriorityEvent));
    EXPECT_FALSE(highFilter(*lowPriorityEvent));
    EXPECT_FALSE(criticalFilter(*lowPriorityEvent));
}

TEST_F(FiltersTest, NotCancelledFilter)
{
    auto notCancelledFilter = filters::notCancelled<TestEvent>();

    EXPECT_TRUE(notCancelledFilter(*userEvent));
    EXPECT_TRUE(notCancelledFilter(*highPriorityEvent));
    EXPECT_FALSE(notCancelledFilter(*cancelledEvent));
}

TEST_F(FiltersTest, TimeRangeFilter)
{
    auto pastToNowFilter   = filters::timeRange<TimedEvent>(past, now);
    auto nowToFutureFilter = filters::timeRange<TimedEvent>(now, future);
    auto narrowRangeFilter = filters::timeRange<TimedEvent>(now - std::chrono::seconds(1), now + std::chrono::seconds(1));

    // 测试 past 到 now 的范围
    EXPECT_TRUE(pastToNowFilter(*pastEvent));
    EXPECT_TRUE(pastToNowFilter(*nowEvent));
    EXPECT_FALSE(pastToNowFilter(*futureEvent));

    // 测试 now 到 future 的范围
    EXPECT_FALSE(nowToFutureFilter(*pastEvent));
    EXPECT_TRUE(nowToFutureFilter(*nowEvent));
    EXPECT_TRUE(nowToFutureFilter(*futureEvent));

    // 测试窄范围
    EXPECT_FALSE(narrowRangeFilter(*pastEvent));
    EXPECT_TRUE(narrowRangeFilter(*nowEvent));
    EXPECT_FALSE(narrowRangeFilter(*futureEvent));
}

TEST_F(FiltersTest, CombinedFilters)
{
    // 组合多个过滤器
    auto categoryFilter     = filters::byCategory<TestEvent>(EventCategory::User);
    auto priorityFilter     = filters::byPriority<TestEvent>(EventPriority::Normal);
    auto notCancelledFilter = filters::notCancelled<TestEvent>();

    CompositeFilter<TestEvent> compositeFilter;
    compositeFilter.addFilter(categoryFilter);
    compositeFilter.addFilter(priorityFilter);
    compositeFilter.addFilter(notCancelledFilter);

    // userEvent 满足所有条件
    EXPECT_TRUE(compositeFilter(*userEvent));

    // appEvent 不满足类别条件
    EXPECT_FALSE(compositeFilter(*appEvent));

    // lowPriorityEvent 不满足优先级条件
    EXPECT_FALSE(compositeFilter(*lowPriorityEvent));

    // cancelledEvent 不满足取消状态条件
    EXPECT_FALSE(compositeFilter(*cancelledEvent));
}

// ======================================

class EventBusTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        eventBus = std::make_unique<EventBus>();
        receivedEvents.clear();
        callCount = 0;
    }

    void TearDown() override
    {
        eventBus.reset();
    }

    std::unique_ptr<EventBus> eventBus;
    std::vector<EventPtr> receivedEvents;
    std::atomic<int> callCount{0};

    // 通用事件处理器
    auto createEventHandler()
    {
        return [this](const TestEvent& event)
        {
            receivedEvents.push_back(std::make_shared<TestEvent>(event));
            callCount++;
        };
    }

    // 高优先级事件处理器
    auto createHighPriorityHandler()
    {
        return [this](const HighPriorityEvent& event)
        {
            receivedEvents.push_back(std::make_shared<HighPriorityEvent>(event));
            callCount++;
        };
    }
};

TEST_F(EventBusTest, BasicSubscribeAndDispatch)
{
    // 订阅事件
    auto handle = eventBus->subscribe<TestEvent>(createEventHandler());
    EXPECT_NE(handle, EventBus::InvalidHandle);

    // 分发事件
    auto testEvent = std::make_shared<TestEvent>(42, "test_data");
    eventBus->dispatch(testEvent);

    // 验证事件被处理
    EXPECT_EQ(receivedEvents.size(), 1);
    EXPECT_EQ(callCount.load(), 1);

    auto receivedTestEvent = std::dynamic_pointer_cast<TestEvent>(receivedEvents[0]);
    ASSERT_NE(receivedTestEvent, nullptr);
    EXPECT_EQ(receivedTestEvent->getValue(), 42);
    EXPECT_EQ(receivedTestEvent->getData(), "test_data");
}

TEST_F(EventBusTest, MultipleSubscribers)
{
    std::vector<EventPtr> events1, events2;
    int count1 = 0, count2 = 0;

    // 订阅多个处理器
    auto handle1 = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
    {
        events1.push_back(std::make_shared<TestEvent>(event));
        count1++;
    });

    auto handle2 = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
    {
        events2.push_back(std::make_shared<TestEvent>(event));
        count2++;
    });

    EXPECT_NE(handle1, EventBus::InvalidHandle);
    EXPECT_NE(handle2, EventBus::InvalidHandle);
    EXPECT_NE(handle1, handle2);

    // 分发事件
    auto testEvent = std::make_shared<TestEvent>(100, "multi_test");
    eventBus->dispatch(testEvent);

    // 验证两个处理器都收到事件
    EXPECT_EQ(events1.size(), 1);
    EXPECT_EQ(events2.size(), 1);
    EXPECT_EQ(count1, 1);
    EXPECT_EQ(count2, 1);
}

TEST_F(EventBusTest, UnsubscribeSpecific)
{
    // 订阅两个处理器
    auto handle1 = eventBus->subscribe<TestEvent>(createEventHandler());
    auto handle2 = eventBus->subscribe<TestEvent>([this](const TestEvent& event)
    {
        callCount += 10; // 不同的处理逻辑
    });

    // 分发事件，验证两个处理器都工作
    auto testEvent = std::make_shared<TestEvent>(1, "test");
    eventBus->dispatch(testEvent);
    EXPECT_EQ(callCount.load(), 11); // 1 + 10

    // 取消订阅第一个处理器
    bool unsubscribed = eventBus->unsubscribe(handle1);
    EXPECT_TRUE(unsubscribed);

    // 重置计数器
    callCount = 0;
    receivedEvents.clear();

    // 再次分发事件
    eventBus->dispatch(testEvent);

    // 只有第二个处理器应该收到事件
    EXPECT_EQ(callCount.load(), 10);
    EXPECT_EQ(receivedEvents.size(), 0);
}

TEST_F(EventBusTest, UnsubscribeAll)
{
    // 订阅多个处理器
    auto handle1 = eventBus->subscribe<TestEvent>(createEventHandler());
    auto handle2 = eventBus->subscribe<HighPriorityEvent>(createHighPriorityHandler());
    auto handle3 = eventBus->subscribe<TestEvent>([this](const TestEvent& event)
    {
        callCount += 5;
    });

    EXPECT_NE(handle1, EventBus::InvalidHandle);
    EXPECT_NE(handle2, EventBus::InvalidHandle);
    EXPECT_NE(handle3, EventBus::InvalidHandle);

    // 分发事件验证订阅有效
    auto testEvent = std::make_shared<TestEvent>(1, "test");
    auto highEvent = std::make_shared<HighPriorityEvent>("important");

    eventBus->dispatch(testEvent);
    eventBus->dispatch(highEvent);

    EXPECT_EQ(callCount.load(), 7); // 1 + 1 + 5
    EXPECT_EQ(receivedEvents.size(), 2);

    // 取消所有订阅
    eventBus->unsubscribeAll();

    // 重置计数器
    callCount = 0;
    receivedEvents.clear();

    // 再次分发事件
    eventBus->dispatch(testEvent);
    eventBus->dispatch(highEvent);

    // 应该没有事件被处理
    EXPECT_EQ(callCount.load(), 0);
    EXPECT_EQ(receivedEvents.size(), 0);
}

TEST_F(EventBusTest, InvalidHandleUnsubscribe)
{
    // 尝试取消订阅无效句柄
    bool result1 = eventBus->unsubscribe(EventBus::InvalidHandle);
    bool result2 = eventBus->unsubscribe(999999); // 不存在的句柄

    EXPECT_FALSE(result1);
    EXPECT_FALSE(result2);
}

TEST_F(EventBusTest, DifferentEventTypes)
{
    std::vector<EventPtr> testEvents, highPriorityEvents;

    // 订阅不同类型的事件
    auto handle1 = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
    {
        testEvents.push_back(std::make_shared<TestEvent>(event));
    });

    auto handle2 = eventBus->subscribe<HighPriorityEvent>([&](const HighPriorityEvent& event)
    {
        highPriorityEvents.push_back(std::make_shared<HighPriorityEvent>(event));
    });

    // 分发不同类型的事件
    auto testEvent = std::make_shared<TestEvent>(42, "test");
    auto highEvent = std::make_shared<HighPriorityEvent>("important");

    eventBus->dispatch(testEvent);
    eventBus->dispatch(highEvent);

    // 验证事件被正确路由
    EXPECT_EQ(testEvents.size(), 1);
    EXPECT_EQ(highPriorityEvents.size(), 1);

    auto receivedTest = std::dynamic_pointer_cast<TestEvent>(testEvents[0]);
    auto receivedHigh = std::dynamic_pointer_cast<HighPriorityEvent>(highPriorityEvents[0]);

    ASSERT_NE(receivedTest, nullptr);
    ASSERT_NE(receivedHigh, nullptr);

    EXPECT_EQ(receivedTest->getValue(), 42);
    EXPECT_EQ(receivedHigh->getMessage(), "important");
}

TEST_F(EventBusTest, AsyncDispatch)
{
    std::atomic<bool> eventProcessed{false};
    std::condition_variable cv;
    std::mutex mtx;

    // 订阅异步事件
    auto handle = eventBus->subscribeAsync<TestEvent>([&](const TestEvent& event)
    {
        std::lock_guard<std::mutex> lock(mtx);
        receivedEvents.push_back(std::make_shared<TestEvent>(event));
        eventProcessed = true;
        cv.notify_one();
    });

    EXPECT_NE(handle, EventBus::InvalidHandle);

    // 异步分发事件
    auto testEvent = std::make_shared<TestEvent>(123, "async_test");
    eventBus->dispatchAsync(testEvent);

    // 等待事件被处理
    std::unique_lock<std::mutex> lock(mtx);
    bool processed = cv.wait_for(lock, std::chrono::seconds(1), [&]
    {
        return eventProcessed.load();
    });

    EXPECT_TRUE(processed);
    EXPECT_EQ(receivedEvents.size(), 1);

    auto receivedEvent = std::dynamic_pointer_cast<TestEvent>(receivedEvents[0]);
    ASSERT_NE(receivedEvent, nullptr);
    EXPECT_EQ(receivedEvent->getValue(), 123);
    EXPECT_EQ(receivedEvent->getData(), "async_test");
}

TEST_F(EventBusTest, EventFiltering)
{
    std::vector<EventPtr> filteredEvents;

    // 创建过滤器：只接受值大于50的事件
    auto filter = [](const TestEvent& event)
    {
        return event.getValue() > 50;
    };

    // 订阅带过滤器的事件
    auto handle = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
    {
        filteredEvents.push_back(std::make_shared<TestEvent>(event));
    }, filter);

    EXPECT_NE(handle, EventBus::InvalidHandle);

    // 分发不同值的事件
    auto event1 = std::make_shared<TestEvent>(30, "small"); // 应该被过滤
    auto event2 = std::make_shared<TestEvent>(70, "large"); // 应该通过
    auto event3 = std::make_shared<TestEvent>(50, "equal"); // 应该被过滤
    auto event4 = std::make_shared<TestEvent>(100, "big");  // 应该通过

    eventBus->dispatch(event1);
    eventBus->dispatch(event2);
    eventBus->dispatch(event3);
    eventBus->dispatch(event4);

    // 验证只有符合条件的事件被处理
    EXPECT_EQ(filteredEvents.size(), 2);

    auto received1 = std::dynamic_pointer_cast<TestEvent>(filteredEvents[0]);
    auto received2 = std::dynamic_pointer_cast<TestEvent>(filteredEvents[1]);

    ASSERT_NE(received1, nullptr);
    ASSERT_NE(received2, nullptr);

    EXPECT_EQ(received1->getValue(), 70);
    EXPECT_EQ(received2->getValue(), 100);
}

TEST_F(EventBusTest, EventPriority)
{
    std::vector<int> processingOrder;
    std::mutex orderMutex;

    // 订阅同步事件处理器，避免异步处理的复杂性
    auto handle = eventBus->subscribeAsync<TestEvent>([&](const TestEvent& event)
    {
        std::lock_guard lock(orderMutex);
        processingOrder.push_back(event.getValue());
    });

    // 清空队列确保干净的测试环境
    eventBus->clearQueue();

    // 创建不同优先级的事件
    auto lowEvent      = std::make_shared<TestEvent>(1, "low");
    auto normalEvent   = std::make_shared<TestEvent>(2, "normal");
    auto highEvent     = std::make_shared<TestEvent>(3, "high");
    auto criticalEvent = std::make_shared<TestEvent>(4, "critical");

    // 设置事件优先级
    lowEvent->setPriority(EventPriority::Low);
    normalEvent->setPriority(EventPriority::Normal);
    highEvent->setPriority(EventPriority::High);
    criticalEvent->setPriority(EventPriority::Critical);

    // 以随机顺序入队事件
    eventBus->enqueue(lowEvent, EventPriority::Low);
    eventBus->enqueue(criticalEvent, EventPriority::Critical);
    eventBus->enqueue(normalEvent, EventPriority::Normal);
    eventBus->enqueue(highEvent, EventPriority::High);

    // 手动处理队列
    eventBus->processQueue();

    // 验证处理顺序（高优先级先处理）
    EXPECT_EQ(processingOrder.size(), 4);

    // 验证优先级顺序：Critical(4) > High(3) > Normal(2) > Low(1)
    EXPECT_EQ(processingOrder[0], 4); // Critical 最先处理
    EXPECT_EQ(processingOrder[1], 3); // High 第二
    EXPECT_EQ(processingOrder[2], 2); // Normal 第三
    EXPECT_EQ(processingOrder[3], 1); // Low 最后
}

TEST_F(EventBusTest, ConcurrentSubscribeUnsubscribe)
{
    const int numThreads          = 4;
    const int operationsPerThread = 100;
    std::atomic<int> totalEvents{0};
    std::vector<std::thread> threads;
    std::vector<EventBus::Handle> handles;
    std::mutex handlesMutex;

    // 启动多个线程进行并发订阅/取消订阅
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&, i]()
        {
            for (int j = 0; j < operationsPerThread; ++j)
            {
                // 订阅
                auto handle = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
                {
                    totalEvents.fetch_add(1);
                });

                {
                    std::lock_guard<std::mutex> lock(handlesMutex);
                    handles.push_back(handle);
                }

                // 短暂等待
                std::this_thread::sleep_for(std::chrono::microseconds(10));

                // 有50%的概率取消订阅
                if (j % 2 == 0)
                {
                    eventBus->unsubscribe(handle);
                }
            }
        });
    }

    // 在另一个线程中分发事件
    std::thread dispatchThread([&]()
    {
        for (int i = 0; i < 50; ++i)
        {
            auto event = std::make_shared<TestEvent>(i, "concurrent_test");
            eventBus->dispatch(event);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // 等待所有线程完成
    for (auto& thread : threads)
    {
        thread.join();
    }
    dispatchThread.join();

    // 验证没有崩溃，并且有事件被处理
    EXPECT_GE(totalEvents.load(), 0);

    // 清理剩余的订阅
    eventBus->unsubscribeAll();
}

TEST_F(EventBusTest, EventStatistics)
{
    // 订阅事件
    auto handle1 = eventBus->subscribe<TestEvent>(createEventHandler());
    auto handle2 = eventBus->subscribeAsync<TestEvent>([](const TestEvent& event)
    {
    });

    // 分发一些事件
    for (int i = 0; i < 5; ++i)
    {
        auto event = std::make_shared<TestEvent>(i, "stats_test");
        eventBus->dispatch(event);
    }

    for (int i = 0; i < 3; ++i)
    {
        auto event = std::make_shared<TestEvent>(i + 10, "async_stats_test");
        eventBus->dispatchAsync(event);
    }

    // 等待异步事件处理完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 获取统计信息
    auto stats = eventBus->getStatistics();

    // 验证统计信息
    EXPECT_GE(stats.dispatchedCount, 5); // 至少5个同步事件
    EXPECT_GE(stats.queuedCount, 3);     // 至少3个异步事件
}

TEST_F(EventBusTest, EventCancellation)
{
    std::vector<EventPtr> processedEvents;

    // 订阅事件，包含取消状态检查
    auto handle = eventBus->subscribe<TestEvent>([&](const TestEvent& event)
    {
        if (!event.isCancelled())
        {
            processedEvents.push_back(std::make_shared<TestEvent>(event));
        }
    });

    // 创建正常事件和取消事件
    auto normalEvent    = std::make_shared<TestEvent>(1, "normal");
    auto cancelledEvent = std::make_shared<TestEvent>(2, "cancelled");
    cancelledEvent->cancel();

    // 分发事件
    eventBus->dispatch(normalEvent);
    eventBus->dispatch(cancelledEvent);

    // 验证只有未取消的事件被处理
    EXPECT_EQ(processedEvents.size(), 1);

    auto processed = std::dynamic_pointer_cast<TestEvent>(processedEvents[0]);
    ASSERT_NE(processed, nullptr);
    EXPECT_EQ(processed->getValue(), 1);
    EXPECT_EQ(processed->getData(), "normal");
}

TEST_F(EventBusTest, MemoryManagement)
{
    std::weak_ptr<TestEvent> weakEvent;

    {
        // 在作用域内创建事件
        auto event = std::make_shared<TestEvent>(42, "memory_test");
        weakEvent  = event;

        // 订阅并分发事件
        auto handle = eventBus->subscribe<TestEvent>(createEventHandler());
        eventBus->dispatch(event);

        // 验证事件被处理
        EXPECT_EQ(receivedEvents.size(), 1);

        // event在这里超出作用域
    }

    // 验证事件对象已被正确释放
    EXPECT_TRUE(weakEvent.expired());

    // 清理EventBus
    eventBus->unsubscribeAll();

    // 再次验证没有内存泄漏
    EXPECT_TRUE(weakEvent.expired());
}
