/**
 * @File SDLWindow.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#include "../Window.hpp"
#include "Core/Utils/Logger.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
class SDLWindow : public Window
{
public:
    SDLWindow(const WindowDesc& desc)
    {
        // 创建SDL窗口
        u32 flags = 0;
        if (desc.resizable)
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        _window = SDL_CreateWindow(
                std::string(desc.title).c_str(),
                static_cast<int>(desc.width),
                static_cast<int>(desc.height),
                flags
                );

        if (!_window)
        {
            XIHE_CORE_ERROR("SDL - 创建窗口失败: {}", SDL_GetError());
            return;
        }

        _width  = desc.width;
        _height = desc.height;
        _title  = desc.title;

        XIHE_CORE_INFO("窗口创建成功: '{}'({}x{})", _width, _height, _title);
    }

    ~SDLWindow() override
    {
        if (_window)
        {
            SDL_DestroyWindow(_window);
            _window = nullptr;
            XIHE_CORE_INFO("窗口已销毁");
        }
    }

    // 窗口显示控制
    void show() override
    {
        if (_window)
        {
            SDL_ShowWindow(_window);
            _visible = true;
        }
    }

    void hide() override
    {
        if (_window)
        {
            SDL_HideWindow(_window);
            _visible = false;
        }
    }

    void close() override
    {
        // 发送关闭事件，实际销毁由析构函数处理
        if (_window)
        {
            // TODO: 发送窗口关闭事件到事件系统
            _shouldClose = true;
        }
    }

    // 窗口属性
    void setTitle(std::string_view title) override
    {
        if (_window)
        {
            SDL_SetWindowTitle(_window, std::string(title).c_str());
            _title = title;
        }
    }

    std::string getTitle() const override
    {
        return _title;
    }

    // 窗口尺寸
    u32 width() const override
    {
        return _width;
    }

    u32 height() const override
    {
        return _height;
    }

    void setSize(u32 width, u32 height) override
    {
        if (_window)
        {
            SDL_SetWindowSize(_window, static_cast<int>(width), static_cast<int>(height));
            _width  = width;
            _height = height;
        }
    }

    // 窗口位置
    void setPosition(i32 x, i32 y) override
    {
        if (_window)
        {
            SDL_SetWindowPosition(_window, x, y);
        }
    }

    // 窗口状态
    bool isVisible() const override
    {
        return _visible;
    }

    bool isMinimized() const override
    {
        if (_window)
        {
            auto flags = SDL_GetWindowFlags(_window);
            return (flags & SDL_WINDOW_MINIMIZED) != 0;
        }
        return false;
    }

    bool isMaximized() const override
    {
        if (_window)
        {
            auto flags = SDL_GetWindowFlags(_window);
            return (flags & SDL_WINDOW_MAXIMIZED) != 0;
        }
        return false;
    }

    void* nativeHandle() const override
    {
        return _window;
    }

    // SDL特有功能
    SDL_Window* getSDLWindow() const
    {
        return _window;
    }

    bool shouldClose() const
    {
        return _shouldClose;
    }

private:
    SDL_Window* _window = nullptr;
    u32 _width          = 0;
    u32 _height         = 0;
    std::string _title;
    bool _visible     = false;
    bool _shouldClose = false;
};

// 工厂函数
std::unique_ptr<Window> CreateSDLWindow(const WindowDesc& desc)
{
    return std::make_unique<SDLWindow>(desc);
}
} // namespace xihe
