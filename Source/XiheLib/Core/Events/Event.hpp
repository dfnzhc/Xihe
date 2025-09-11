/**
 * @File Event.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/26
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <typeindex>
#include <string>
#include <chrono>
#include <concepts>
#include <functional>

#include "Core/Base/Defines.hpp"
#include "Core/Base/Concepts.hpp"
#include "Core/Utils/Enum.hpp"
#include "Core/Utils/Time/Clock.hpp"
#include "Core/Memory/Memory.hpp"

namespace xihe {
enum class EventCategory : u32
{
    None     = 0,
    Input    = 1u << 0,
    Window   = 1u << 1,
    App      = 1u << 2,
    Timer    = 1u << 3,
    Renderer = 1u << 4,
    User     = 1u << 7,
};

XIHE_ENUM_CLASS_OPERATORS(EventCategory);

enum class EventPriority : u32
{
    Low      = 0,
    Normal   = 1,
    High     = 2,
    Critical = 3
};

// clang-format off
// 事件接口类型
class IEvent
{
public:
    virtual ~IEvent() = default;

    // 获取事件类型ID
    XIHE_NODISCARD virtual std::type_index typeId() const = 0;

    // 获取事件优先级
    XIHE_NODISCARD virtual EventPriority getPriority() const { return EventPriority::Normal; }

    // 设置事件优先级
    virtual void setPriority(EventPriority priority) = 0;

    // 获取事件分类
    XIHE_NODISCARD virtual EventCategory getCategory() const { return EventCategory::None; }

    // 获取时间戳
    XIHE_NODISCARD virtual TimePoint getTimestamp() const { return Clock::now(); }

    // 调试信息
    XIHE_NODISCARD virtual std::string toString() const { return "Unknown Event"; }

    // 事件是否可以被取消
    XIHE_NODISCARD virtual bool isCancellable() const { return false; }

    // 取消事件
    virtual void cancel() { }

    // 检查事件是否被取消
    XIHE_NODISCARD virtual bool isCancelled() const { return false; }
};
// clang-format on

using EventPtr      = SharedPtr<IEvent>;
using EventConstPtr = SharedPtr<const IEvent>;

template <typename E>
concept cEventType = requires(E t)
{
    { t.typeId() } -> std::convertible_to<std::type_index>;
    std::is_base_of_v<IEvent, E>;
    std::is_copy_constructible_v<E>;
    std::is_move_constructible_v<E>;
};

// 类型安全的事件类型ID获取
template <cEventType E>
constexpr std::type_index EventTypeId()
{
    return std::type_index(typeid(E));
}

// clang-format off
// 事件基础实现模板
template <typename Derived>
class EventBase : public IEvent
{
public:
    XIHE_NODISCARD std::type_index typeId() const override { return std::type_index(typeid(Derived)); }

    XIHE_NODISCARD std::string toString() const override { return typeid(Derived).name(); }
    
    XIHE_NODISCARD TimePoint getTimestamp() const override { return _timestamp; }

    void setPriority(EventPriority priority) override { _priority = priority; }
    XIHE_NODISCARD EventPriority getPriority() const override { return _priority; }

    void setCategory(EventCategory category) { _category = category; }
    XIHE_NODISCARD EventCategory getCategory() const override { return _category; }
    
    XIHE_NODISCARD bool isCancellable() const override { return true; }
    XIHE_NODISCARD bool isCancelled() const override { return _cancelled; }
    void cancel() override { _cancelled = true; }

protected:
    EventPriority _priority{EventPriority::Normal};
    EventCategory _category{EventCategory::None};
    TimePoint _timestamp{Clock::now()};
    bool _cancelled{false};
};
// clang-format on

// ======================================
// 回调函数类型定义

template <cEventType E>
using EventCallback = std::function<void(const E&)>;

template <cEventType E>
using EventFilter = std::function<bool(const E&)>;

// 通用事件回调（用于内部实现）
using GenericEventCallback = std::function<void(const EventPtr&)>;

// 类型安全的回调包装器

template <cEventType E>
GenericEventCallback WrapCallback(EventCallback<E> inCallback)
{
    return [callback = std::move(inCallback)](const EventPtr& baseEvent)
    {
        if (auto specificEvent = std::dynamic_pointer_cast<const E>(baseEvent))
        {
            callback(*specificEvent);
        }
    };
}

template <cEventType E>
std::function<bool(const EventPtr&)> WrapFilter(EventFilter<E> inFilter)
{
    return [filter = std::move(inFilter)](const EventPtr& baseEvent) -> bool
    {
        if (auto specificEvent = std::dynamic_pointer_cast<const E>(baseEvent))
        {
            return filter(*specificEvent);
        }
        return false;
    };
}

using Handle                    = u64;
constexpr Handle kInvalidHandle = 0;
} // namespace xihe
