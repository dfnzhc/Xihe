/**
 * @File EventBus.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include "EventBus.hpp"
#include <algorithm>

using namespace xihe;

EventBus::Handle EventBus::subscribe(EventType type, Callback cb)
{
    std::lock_guard lk(_mtx);
    const Handle h = _next++;
    _byType[type].push_back(Listener{h, std::move(cb)});
    return h;
}

EventBus::Handle EventBus::subscribeCategory(EventCategory cat, Callback cb)
{
    std::lock_guard lk(_mtx);
    const Handle h = _next++;
    _byCategory[cat].push_back(Listener{h, std::move(cb)});
    return h;
}

bool EventBus::unsubscribe(Handle h)
{
    std::lock_guard lk(_mtx);
    bool removed = false;
    for (auto& [t, vec] : _byType)
    {
        auto it = std::remove_if(vec.begin(), vec.end(), [&](const Listener& l)
        {
            return l.id == h;
        });
        if (it != vec.end())
        {
            vec.erase(it, vec.end());
            removed = true;
        }
    }
    for (auto& [c, vec] : _byCategory)
    {
        auto it = std::remove_if(vec.begin(), vec.end(), [&](const Listener& l)
        {
            return l.id == h;
        });
        if (it != vec.end())
        {
            vec.erase(it, vec.end());
            removed = true;
        }
    }
    return removed;
}

void EventBus::emit(const Event& e) const
{
    std::vector<Callback> toCall;
    {
        std::lock_guard lk(_mtx);
        if (auto it = _byType.find(e.header.type); it != _byType.end())
        {
            for (auto& l : it->second)
                toCall.push_back(l.cb);
        }
        for (auto& [cat, vec] : _byCategory)
        {
            if (IsSet(cat, e.header.category))
            {
                for (auto& l : vec)
                    toCall.push_back(l.cb);
            }
        }
    }
    for (auto& cb : toCall)
        cb(e);
}

void EventBus::post(Event e)
{
    std::lock_guard lk(_mtx);
    _queue.emplace_back(e);
}

void EventBus::dispatch()
{
    std::deque<Event> local;
    {
        std::lock_guard lk(_mtx);
        local.swap(_queue);
    }
    for (auto& e : local)
    {
        emit(e);
    }
}
