/**
 * @File SDLInput.cpp
 */

#include <array>
#include <atomic>

#include "Base/Concepts.hpp"
#include "Core/Base/Defines.hpp"
#include "Platform/Input.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
static constexpr Size kKeyCount = 512;

class SDLInput final : public IInput
{
public:
    SDLInput()
    {
        _keysCurrent.fill(false);
        _keysPrevious.fill(false);
        _mouseState = {};
    }

    void handleSDLEvent(const SDL_Event& e)
    {
        switch (e.type) {
        case SDL_EVENT_KEY_DOWN: if (e.key.scancode < kKeyCount) { _keysCurrent[e.key.scancode] = true; }
            break;
        case SDL_EVENT_KEY_UP: if (e.key.scancode < kKeyCount) { _keysCurrent[e.key.scancode] = false; }
            break;
        case SDL_EVENT_MOUSE_MOTION: _mouseState.x = e.motion.x;
            _mouseState.y = e.motion.y;
            break;
        case SDL_EVENT_MOUSE_WHEEL: _mouseState.wheel += As<i32>(e.wheel.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            const bool down = (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            const int btn = As<int>(e.button.button);
            switch (btn) {
            case SDL_BUTTON_LEFT: _mouseState.buttons[static_cast<int>(MouseButton::Left)] = down;
                break;
            case SDL_BUTTON_RIGHT: _mouseState.buttons[static_cast<int>(MouseButton::Right)] = down;
                break;
            case SDL_BUTTON_MIDDLE: _mouseState.buttons[static_cast<int>(MouseButton::Middle)] = down;
                break;
            default: break;
            }
            break;
        }
        default: break;
        }
    }

    XIHE_NODISCARD bool isKeyDown(KeyCode key) const override
    {
        const auto sc = static_cast<SDL_Scancode>(static_cast<u32>(key));
        if (static_cast<Size>(sc) >= kKeyCount) { return false; }
        return _keysCurrent[sc];
    }

    XIHE_NODISCARD bool wasKeyPressed(KeyCode key) const override
    {
        const auto sc = static_cast<SDL_Scancode>(static_cast<u32>(key));
        if (static_cast<Size>(sc) >= kKeyCount) { return false; }
        return _keysCurrent[sc] && !_keysPrevious[sc];
    }

    XIHE_NODISCARD MouseState mouseState() const override { return _mouseState; }

    void newFrame() override
    {
        _keysPrevious = _keysCurrent;
        _mouseState.wheel = 0; // 滚轮按帧计数
    }

private:
    std::array<bool, kKeyCount> _keysCurrent{};
    std::array<bool, kKeyCount> _keysPrevious{};
    MouseState _mouseState{};
};

IInput* CreateSDLInputSingleton()
{
    static SDLInput s_instance;
    return &s_instance;
}

void SDLInputHandleEvent(const SDL_Event& e) { static_cast<SDLInput*>(CreateSDLInputSingleton())->handleSDLEvent(e); }
} // namespace xihe
