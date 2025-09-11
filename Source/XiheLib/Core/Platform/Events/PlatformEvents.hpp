/**
 * @File PlatformEvents.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/9/11
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <string>
#include <memory>
#include <format>
#include "Core/Base/Defines.hpp"
#include "Core/Events/Event.hpp"
#include "Core/Platform/Input.hpp"

namespace xihe {
class KeyboardEvent : public EventBase<KeyboardEvent>
{
public:
    KeyCode key;
    bool pressed;
    bool repeat;
    u16 scancode;

    KeyboardEvent(KeyCode k, bool p, bool r = false, u16 sc = 0) :
        key(k), pressed(p), repeat(r), scancode(sc)
    {
        _category = EventCategory::Input;
        _priority = EventPriority::High;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("KeyboardEvent: key={}, pressed={}, repeat={}", static_cast<u32>(key), pressed, repeat);
    }
};

class TextInputEvent : public EventBase<TextInputEvent>
{
public:
    std::string text;

    explicit TextInputEvent(std::string t) :
        text(std::move(t))
    {
        _category = EventCategory::Input;
        _priority = EventPriority::Normal;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("TextInputEvent: text='{}'", text);
    }
};

class TextEditingEvent : public EventBase<TextEditingEvent>
{
public:
    std::string text;
    i32 start;
    i32 length;

    TextEditingEvent(std::string t, i32 s, i32 l) :
        text(std::move(t)), start(s), length(l)
    {
        _category = EventCategory::Input;
        _priority = EventPriority::Normal;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("TextEditingEvent: text='{}', start={}, length={}", text, start, length);
    }
};

// -----------------------------

class MouseButtonEvent : public EventBase<MouseButtonEvent>
{
public:
    MouseButton button;
    bool pressed;
    i32 x, y;
    u8 clicks;

    MouseButtonEvent(MouseButton b, bool p, i32 x_, i32 y_, u8 c = 1) :
        button(b), pressed(p), x(x_), y(y_), clicks(c)
    {
        _category = EventCategory::Input;
        _priority = EventPriority::High;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("MouseButtonEvent: button={}, pressed={}, pos=({},{}), clicks={}", static_cast<u32>(button),
                           pressed, x, y, clicks);
    }
};

class MouseMotionEvent : public EventBase<MouseMotionEvent>
{
public:
    i32 x, y;
    i32 deltaX, deltaY;

    MouseMotionEvent(i32 x_, i32 y_, i32 dx, i32 dy) :
        x(x_), y(y_), deltaX(dx), deltaY(dy)
    {
        _category = EventCategory::Input;
        _priority = EventPriority::Normal;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("MouseMotionEvent: pos=({},{}), delta=({},{})", x, y, deltaX, deltaY);
    }
};

class MouseWheelEvent : public EventBase<MouseWheelEvent>
{
public:
    f32 deltaX, deltaY;
    i32 x, y;
    bool flipped;

    MouseWheelEvent(f32 dx, f32 dy, i32 x_, i32 y_, bool f = false) :
        deltaX(dx), deltaY(dy), x(x_), y(y_), flipped(f)
    {
        _category = EventCategory::Input;
        _priority = EventPriority::Normal;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("MouseWheelEvent: delta=({},{}), pos=({},{})", deltaX, deltaY, x, y);
    }
};

// -----------------------------

class WindowCloseEvent : public EventBase<WindowCloseEvent>
{
public:
    u32 windowId;

    explicit WindowCloseEvent(u32 id) :
        windowId(id)
    {
        _category = EventCategory::Window;
        _priority = EventPriority::Critical;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("WindowCloseEvent: windowId={}", windowId);
    }
};

class WindowResizeEvent : public EventBase<WindowResizeEvent>
{
public:
    u32 windowId;
    u32 width, height;

    WindowResizeEvent(u32 id, u32 w, u32 h) :
        windowId(id), width(w), height(h)
    {
        _category = EventCategory::Window;
        _priority = EventPriority::High;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("WindowResizeEvent: windowId={}, size={}x{}", windowId, width, height);
    }
};

class WindowFocusEvent : public EventBase<WindowFocusEvent>
{
public:
    u32 windowId;
    bool gained; // true=获得焦点, false=失去焦点

    WindowFocusEvent(u32 id, bool g) :
        windowId(id), gained(g)
    {
        _category = EventCategory::Window;
        _priority = EventPriority::Normal;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("WindowFocusEvent: windowId={}, gained={}", windowId, gained);
    }
};

class WindowMoveEvent : public EventBase<WindowMoveEvent>
{
public:
    u32 windowId;
    i32 x, y; // 新的窗口位置

    WindowMoveEvent(u32 id, i32 x_, i32 y_) :
        windowId(id), x(x_), y(y_)
    {
        _category = EventCategory::Window;
        _priority = EventPriority::Low;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return std::format("WindowMoveEvent: windowId={}, pos=({},{})", windowId, x, y);
    }
};

// -----------------------------

class AppQuitEvent : public EventBase<AppQuitEvent>
{
public:
    AppQuitEvent()
    {
        _category = EventCategory::App;
        _priority = EventPriority::Critical;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return "AppQuitEvent";
    }
};

class AppLowMemoryEvent : public EventBase<AppLowMemoryEvent>
{
public:
    AppLowMemoryEvent()
    {
        _category = EventCategory::App;
        _priority = EventPriority::High;
    }

    XIHE_NODISCARD std::string toString() const override
    {
        return "AppLowMemoryEvent";
    }
};

// -----------------------------

using KeyboardEventPtr     = SharedPtr<KeyboardEvent>;
using TextInputEventPtr    = SharedPtr<TextInputEvent>;
using TextEditingEventPtr  = SharedPtr<TextEditingEvent>;
using MouseButtonEventPtr  = SharedPtr<MouseButtonEvent>;
using MouseMotionEventPtr  = SharedPtr<MouseMotionEvent>;
using MouseWheelEventPtr   = SharedPtr<MouseWheelEvent>;
using WindowCloseEventPtr  = SharedPtr<WindowCloseEvent>;
using WindowResizeEventPtr = SharedPtr<WindowResizeEvent>;
using WindowFocusEventPtr  = SharedPtr<WindowFocusEvent>;
using WindowMoveEventPtr   = SharedPtr<WindowMoveEvent>;
using AppQuitEventPtr      = SharedPtr<AppQuitEvent>;
using AppLowMemoryEventPtr = SharedPtr<AppLowMemoryEvent>;
} // namespace xihe
