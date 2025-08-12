/**
 * @File Window.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <string_view>

#include "Core/Base/Defines.hpp"

namespace xihe {
struct WindowDesc
{
    u32 width{1280};
    u32 height{720};
    bool resizable{true};
    std::string_view title{"Xihe"};
};

enum class EventType : u32
{
    None = 0,
    Close,
    Resize,
    KeyDown,
    KeyUp,
    MouseDown,
    MouseUp,
    MouseMove,
    MouseWheel,
    TextInput,
};

struct Event
{
    EventType type{EventType::None};
    u32 param0{0};
    u32 param1{0};
    u32 param2{0};
};

class XIHE_API IWindow
{
public:
    virtual ~IWindow() = default;

    virtual void show() = 0;
    virtual void setTitle(std::string_view newTitle) = 0;

    virtual bool pollEvent(Event& outEvent) = 0;

    XIHE_NODISCARD virtual void* nativeHandle() const = 0;

    XIHE_NODISCARD virtual u32 width() const = 0;
    XIHE_NODISCARD virtual u32 height() const = 0;
};
} // namespace xihe
