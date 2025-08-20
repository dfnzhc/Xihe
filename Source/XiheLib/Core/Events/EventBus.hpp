/**
 * @File EventBus.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
#include <atomic>

#include "Core/Base/Defines.hpp"
#include "Core/Base/Error.hpp"
#include "Core/Events/Event.hpp"

namespace xihe {
// TODO: 考虑使用 eventpp
// TODO: 多线程调度事件
class XIHE_API EventBus
{
public:
    using Callback = std::function<void(const Event&)>;
    using Handle   = u64;

    EventBus() = default;

    // 订阅具体类型
    Handle subscribe(EventType type, Callback cb);
    // 订阅类别（将收到该类别下的所有事件）
    Handle subscribeCategory(EventCategory cat, Callback cb);
    bool unsubscribe(Handle h);

    // 同线程立即派发
    void emit(const Event& e) const;

    // 跨线程投递（排队，主线程在 dispatch 中处理）
    void post(Event e);
    void dispatch();

private:
    struct Listener
    {
        Handle id{0};
        Callback cb{};
    };

    // 映射：具体类型 -> 监听列表
    std::unordered_map<EventType, std::vector<Listener>> _byType;
    // 类别订阅
    std::unordered_map<EventCategory, std::vector<Listener>> _byCategory;

    // 排队事件
    std::deque<Event> _queue;
    mutable std::mutex _mtx;
    std::atomic<Handle> _next{1};
};
} // namespace xihe
