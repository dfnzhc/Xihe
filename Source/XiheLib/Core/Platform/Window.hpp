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
#include "Core/Events/Event.hpp"

namespace xihe {
struct WindowDesc
{
    u32 width{1280};
    u32 height{720};
    bool resizable{true};
    std::string_view title{"Xihe"};
};

enum class CursorMode : u8
{
    Normal = 0,
    Relative
};

class XIHE_API IWindow
{
public:
    virtual ~IWindow() = default;

    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void setTitle(std::string_view newTitle) = 0;

    virtual bool pollEvent(Event& event) = 0;

    // 控制与属性
    virtual void minimize() = 0;
    virtual void maximize() = 0;
    virtual void restore() = 0;
    virtual void setSize(u32 w, u32 h) = 0;
    virtual void setPosition(i32 x, i32 y) = 0;
    virtual void setResizable(bool resizable) = 0;
    virtual void setFullscreen(bool fullscreen) = 0;
    virtual void setCursorVisible(bool visible) = 0;
    virtual void setCursorMode(CursorMode mode) = 0;

    XIHE_NODISCARD virtual void* nativeHandle() const = 0;

    XIHE_NODISCARD virtual u32 width() const = 0;
    XIHE_NODISCARD virtual u32 height() const = 0;

    XIHE_NODISCARD virtual float dpi() const = 0;          // 逻辑 DPI（近似）
    XIHE_NODISCARD virtual float contentScale() const = 0; // 像素/逻辑尺寸比例
};
} // namespace xihe
