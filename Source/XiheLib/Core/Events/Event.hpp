/**
 * @File Event.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <variant>
#include <string>

#include "Core/Base/Defines.hpp"
#include "Core/Utils/Enum.hpp"
#include "Core/Utils/Time/Clock.hpp"
#include "Platform/Input.hpp"

namespace xihe {
// ======================================
// 分类与类型
// ======================================

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

enum class EventType : u32
{
    // Input
    KeyDown,
    KeyUp,
    TextInput,
    MouseMove,
    MouseDown,
    MouseUp,
    MouseWheel,

    // Window
    WindowResize,
    WindowCloseRequested,
    WindowFocusChanged,
};

// ======================================
// 载荷（Payload）
// ======================================

struct KeyDownEvent
{
    KeyCode key{};
    bool repeat{false};
};

struct KeyUpEvent
{
    KeyCode key{};
};

struct TextInputEvent
{
    u32 codepoint{0};
};

struct MouseMoveEvent
{
    i32 x{0};
    i32 y{0};
    i32 dx{0};
    i32 dy{0};
};

struct MouseDownEvent
{
    MouseButton button{MouseButton::Left};
    i32 x{0};
    i32 y{0};
    u8 clicks{1};
};

struct MouseUpEvent
{
    MouseButton button{MouseButton::Left};
    i32 x{0};
    i32 y{0};
};

struct MouseWheelEvent
{
    f32 deltaX{0};
    f32 deltaY{0};
    i32 x{0};
    i32 y{0};
};

struct WindowResizeEvent
{
    u32 width{0};
    u32 height{0};
    f32 dpi{0.0f};
};

struct WindowCloseRequestedEvent
{
};

struct WindowFocusChangedEvent
{
    bool focused{false};
};

using EventPayload = std::variant<
    KeyDownEvent,
    KeyUpEvent,
    TextInputEvent,
    MouseMoveEvent,
    MouseDownEvent,
    MouseUpEvent,
    MouseWheelEvent,
    WindowResizeEvent,
    WindowCloseRequestedEvent,
    WindowFocusChangedEvent>;

// ======================================
// 事件头与事件体
// ======================================

struct EventHeader
{
    EventType type{EventType::KeyDown};
    EventCategory category{EventCategory::None};
    TimePoint timestamp{}; // steady clock
};

struct Event
{
    EventHeader header{};
    EventPayload payload{};

    template <typename T>
    XIHE_NODISCARD const T* as() const noexcept
    {
        return std::get_if<T>(&payload);
    }

    template <typename T>
    XIHE_NODISCARD bool is() const noexcept
    {
        return std::holds_alternative<T>(payload);
    }
};
} // namespace xihe
