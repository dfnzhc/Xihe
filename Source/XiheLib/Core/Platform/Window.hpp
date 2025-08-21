/**
 * @File Window.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <string_view>
#include <memory>
#include <string>

#include "Core/Base/Defines.hpp"

namespace xihe {
struct WindowDesc
{
    u32 width{1280};
    u32 height{720};
    bool resizable{true};
    std::string_view title{"Xihe"};
};

// IWindow 抽象接口
class XIHE_API Window
{
public:
    virtual ~Window() = default;

    // 窗口显示控制
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void close() = 0;

    // 窗口属性
    virtual void setTitle(std::string_view title) = 0;
    XIHE_NODISCARD virtual std::string getTitle() const = 0;

    // 窗口尺寸
    XIHE_NODISCARD virtual u32 width() const = 0;
    XIHE_NODISCARD virtual u32 height() const = 0;
    virtual void setSize(u32 width, u32 height) = 0;

    // 窗口位置
    virtual void setPosition(i32 x, i32 y) = 0;

    // 窗口状态
    XIHE_NODISCARD virtual bool isVisible() const = 0;
    XIHE_NODISCARD virtual bool isMinimized() const = 0;
    XIHE_NODISCARD virtual bool isMaximized() const = 0;

    // 原生句柄
    XIHE_NODISCARD virtual void* nativeHandle() const = 0;
};
} // namespace xihe
