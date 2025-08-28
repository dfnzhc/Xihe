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

// 批量事件处理器
class XIHE_API BatchEventProcessor
{
public:
    BatchEventProcessor(Size batchSize                    = 100,
                        std::chrono::milliseconds timeout = std::chrono::milliseconds(10));

    void setProcessor(std::function<void(const std::vector<EventPtr>&)> processor);
    void addEvent(EventPtr event);
    void flush();
    Size pendingCount() const;

private:
    std::vector<EventPtr> _batch;
    Size _batchSize;
    std::chrono::milliseconds _timeout;
    TimePoint _lastFlush;
    std::function<void(const std::vector<EventPtr>&)> _processor;
};

} // namespace xihe
