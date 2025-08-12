/**
 * @File SDLWindow.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#include <memory>
#include <string>


#include "Platform/Window.hpp"
#include "Platform/Input.hpp"
#include "Core/Base/Defines.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
class SDLWindow final : public IWindow
{
public:
    explicit SDLWindow(const WindowDesc& desc)
    {
        Uint32 flags = 0;
        if (desc.resizable) { flags |= SDL_WINDOW_RESIZABLE; }

        _window = SDL_CreateWindow(desc.title.data(), static_cast<int>(desc.width), static_cast<int>(desc.height), flags);
        if (_window) {
            _width = desc.width;
            _height = desc.height;
        }
    }

    ~SDLWindow() override
    {
        if (_window) {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }
    }

    void show() override { if (_window) { SDL_ShowWindow(_window); } }
    void hide() override { if (_window) { SDL_HideWindow(_window); } }

    void setTitle(std::string_view newTitle) override { if (_window) { SDL_SetWindowTitle(_window, std::string(newTitle).c_str()); } }

    bool pollEvent(Event& outEvent) override
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            extern void SDLInputHandleEvent(const SDL_Event&);
            SDLInputHandleEvent(e);
            switch (e.type) {
            case SDL_EVENT_QUIT: outEvent.type = EventType::Close;
                return true;
            case SDL_EVENT_WINDOW_RESIZED: if (e.window.windowID == SDL_GetWindowID(_window)) {
                    _width = static_cast<u32>(e.window.data1);
                    _height = static_cast<u32>(e.window.data2);
                    outEvent.type = EventType::Resize;
                    outEvent.param0 = _width;
                    outEvent.param1 = _height;
                    return true;
                }
                break;
            case SDL_EVENT_TEXT_INPUT: outEvent.type = EventType::TextInput;
                if (e.text.text[0] != '\0') { outEvent.param0 = static_cast<u32>(static_cast<unsigned char>(e.text.text[0])); }
                else { outEvent.param0 = 0; }
                return true;
            default: break;
            }
        }
        return false;
    }

    // 控制与属性
    void minimize() override { if (_window) { SDL_MinimizeWindow(_window); } }
    void maximize() override { if (_window) { SDL_MaximizeWindow(_window); } }
    void restore() override { if (_window) { SDL_RestoreWindow(_window); } }
    void setSize(u32 w, u32 h) override { if (_window) { SDL_SetWindowSize(_window, static_cast<int>(w), static_cast<int>(h)); } }
    void setPosition(i32 x, i32 y) override { if (_window) { SDL_SetWindowPosition(_window, x, y); } }
    void setResizable(bool resizable) override { if (_window) { SDL_SetWindowResizable(_window, resizable); } }
    void setFullscreen(bool fullscreen) override { if (_window) { SDL_SetWindowFullscreen(_window, fullscreen); } }

    void setCursorVisible(bool visible) override
    {
        if (visible)
            SDL_ShowCursor();
        else
            SDL_HideCursor();
    }

    void setCursorMode(CursorMode mode) override { if (_window) { SDL_SetWindowRelativeMouseMode(_window, mode == CursorMode::Relative); } }

    XIHE_NODISCARD void* nativeHandle() const override
    {
        // XXX: SDL3 存在 SDL_GetWindowFromID 具体实现，暂时保持类型擦除实现
        return reinterpret_cast<void*>(_window);
    }

    XIHE_NODISCARD u32 width() const override { return _width; }
    XIHE_NODISCARD u32 height() const override { return _height; }

    XIHE_NODISCARD float dpi() const override
    {
        float ddpi = 0.f;
        if (_window) { ddpi = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(_window)); }
        return ddpi;
    }

    XIHE_NODISCARD float contentScale() const override
    {
        int pixelW = 0, pixelH = 0;
        float sx = 1.0f, sy = 1.0f;
        if (_window) { SDL_GetWindowSizeInPixels(_window, &pixelW, &pixelH); }
        if (_width != 0 && _height != 0) {
            sx = static_cast<float>(pixelW) / static_cast<float>(_width);
            sy = static_cast<float>(pixelH) / static_cast<float>(_height);
        }
        return (sx + sy) * 0.5f;
    }

private:
    SDL_Window* _window{nullptr};
    u32 _width{0};
    u32 _height{0};
};

std::unique_ptr<IWindow> CreateSDLWindow(const WindowDesc& desc) { return std::make_unique<SDLWindow>(desc); }
} // namespace xihe
