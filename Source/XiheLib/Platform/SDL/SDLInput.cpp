/**
 * @File SDLInput.cpp
 */

#include <array>
#include <atomic>

#include "Core/Base/Defines.hpp"
#include "Platform/Input.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
static constexpr Size kKeyCount = 512;

static SDL_Scancode MapKeyCodeToSDLScancode(KeyCode key)
{
    const u32 code = static_cast<u32>(key);
    // clang-format off
    if ((code >= 'A' && code <= 'Z') || (code >= '0' && code <= '9')) {
        switch (code) {
        case 'A': return SDL_SCANCODE_A; case 'B': return SDL_SCANCODE_B; case 'C': return SDL_SCANCODE_C; case 'D': return SDL_SCANCODE_D;
        case 'E': return SDL_SCANCODE_E; case 'F': return SDL_SCANCODE_F; case 'G': return SDL_SCANCODE_G; case 'H': return SDL_SCANCODE_H;
        case 'I': return SDL_SCANCODE_I; case 'J': return SDL_SCANCODE_J; case 'K': return SDL_SCANCODE_K; case 'L': return SDL_SCANCODE_L;
        case 'M': return SDL_SCANCODE_M; case 'N': return SDL_SCANCODE_N; case 'O': return SDL_SCANCODE_O; case 'P': return SDL_SCANCODE_P;
        case 'Q': return SDL_SCANCODE_Q; case 'R': return SDL_SCANCODE_R; case 'S': return SDL_SCANCODE_S; case 'T': return SDL_SCANCODE_T;
        case 'U': return SDL_SCANCODE_U; case 'V': return SDL_SCANCODE_V; case 'W': return SDL_SCANCODE_W; case 'X': return SDL_SCANCODE_X;
        case 'Y': return SDL_SCANCODE_Y; case 'Z': return SDL_SCANCODE_Z;
        case '0': return SDL_SCANCODE_0; case '1': return SDL_SCANCODE_1; case '2': return SDL_SCANCODE_2; case '3': return SDL_SCANCODE_3;
        case '4': return SDL_SCANCODE_4; case '5': return SDL_SCANCODE_5; case '6': return SDL_SCANCODE_6; case '7': return SDL_SCANCODE_7;
        case '8': return SDL_SCANCODE_8; case '9': return SDL_SCANCODE_9;
        default: break;
        }
    }
    // clang-format on

    switch (key) {
    case KeyCode::Space: return SDL_SCANCODE_SPACE;
    case KeyCode::Apostrophe: return SDL_SCANCODE_APOSTROPHE;
    case KeyCode::Comma: return SDL_SCANCODE_COMMA;
    case KeyCode::Minus: return SDL_SCANCODE_MINUS;
    case KeyCode::Period: return SDL_SCANCODE_PERIOD;
    case KeyCode::Slash: return SDL_SCANCODE_SLASH;
    case KeyCode::Semicolon: return SDL_SCANCODE_SEMICOLON;
    case KeyCode::Equal: return SDL_SCANCODE_EQUALS;
    case KeyCode::LeftBracket: return SDL_SCANCODE_LEFTBRACKET;
    case KeyCode::Backslash: return SDL_SCANCODE_BACKSLASH;
    case KeyCode::RightBracket: return SDL_SCANCODE_RIGHTBRACKET;
    case KeyCode::GraveAccent: return SDL_SCANCODE_GRAVE;

    case KeyCode::Escape: return SDL_SCANCODE_ESCAPE;
    case KeyCode::Tab: return SDL_SCANCODE_TAB;
    case KeyCode::Enter: return SDL_SCANCODE_RETURN;
    case KeyCode::Backspace: return SDL_SCANCODE_BACKSPACE;
    case KeyCode::Insert: return SDL_SCANCODE_INSERT;
    case KeyCode::Del: return SDL_SCANCODE_DELETE;
    case KeyCode::Right: return SDL_SCANCODE_RIGHT;
    case KeyCode::Left: return SDL_SCANCODE_LEFT;
    case KeyCode::Down: return SDL_SCANCODE_DOWN;
    case KeyCode::Up: return SDL_SCANCODE_UP;
    case KeyCode::PageUp: return SDL_SCANCODE_PAGEUP;
    case KeyCode::PageDown: return SDL_SCANCODE_PAGEDOWN;
    case KeyCode::Home: return SDL_SCANCODE_HOME;
    case KeyCode::End: return SDL_SCANCODE_END;
    case KeyCode::CapsLock: return SDL_SCANCODE_CAPSLOCK;
    case KeyCode::ScrollLock: return SDL_SCANCODE_SCROLLLOCK;
    case KeyCode::NumLock: return SDL_SCANCODE_NUMLOCKCLEAR;
    case KeyCode::PrintScreen: return SDL_SCANCODE_PRINTSCREEN;
    case KeyCode::Pause: return SDL_SCANCODE_PAUSE;
    case KeyCode::F1: return SDL_SCANCODE_F1;
    case KeyCode::F2: return SDL_SCANCODE_F2;
    case KeyCode::F3: return SDL_SCANCODE_F3;
    case KeyCode::F4: return SDL_SCANCODE_F4;
    case KeyCode::F5: return SDL_SCANCODE_F5;
    case KeyCode::F6: return SDL_SCANCODE_F6;
    case KeyCode::F7: return SDL_SCANCODE_F7;
    case KeyCode::F8: return SDL_SCANCODE_F8;
    case KeyCode::F9: return SDL_SCANCODE_F9;
    case KeyCode::F10: return SDL_SCANCODE_F10;
    case KeyCode::F11: return SDL_SCANCODE_F11;
    case KeyCode::F12: return SDL_SCANCODE_F12;
    case KeyCode::Keypad0: return SDL_SCANCODE_KP_0;
    case KeyCode::Keypad1: return SDL_SCANCODE_KP_1;
    case KeyCode::Keypad2: return SDL_SCANCODE_KP_2;
    case KeyCode::Keypad3: return SDL_SCANCODE_KP_3;
    case KeyCode::Keypad4: return SDL_SCANCODE_KP_4;
    case KeyCode::Keypad5: return SDL_SCANCODE_KP_5;
    case KeyCode::Keypad6: return SDL_SCANCODE_KP_6;
    case KeyCode::Keypad7: return SDL_SCANCODE_KP_7;
    case KeyCode::Keypad8: return SDL_SCANCODE_KP_8;
    case KeyCode::Keypad9: return SDL_SCANCODE_KP_9;
    case KeyCode::KeypadDel: return SDL_SCANCODE_KP_PERIOD;
    case KeyCode::KeypadDivide: return SDL_SCANCODE_KP_DIVIDE;
    case KeyCode::KeypadMultiply: return SDL_SCANCODE_KP_MULTIPLY;
    case KeyCode::KeypadSubtract: return SDL_SCANCODE_KP_MINUS;
    case KeyCode::KeypadAdd: return SDL_SCANCODE_KP_PLUS;
    case KeyCode::KeypadEnter: return SDL_SCANCODE_KP_ENTER;
    case KeyCode::KeypadEqual: return SDL_SCANCODE_KP_EQUALS;
    case KeyCode::LeftShift: return SDL_SCANCODE_LSHIFT;
    case KeyCode::LeftControl: return SDL_SCANCODE_LCTRL;
    case KeyCode::LeftAlt: return SDL_SCANCODE_LALT;
    case KeyCode::LeftSuper: return SDL_SCANCODE_LGUI;
    case KeyCode::RightShift: return SDL_SCANCODE_RSHIFT;
    case KeyCode::RightControl: return SDL_SCANCODE_RCTRL;
    case KeyCode::RightAlt: return SDL_SCANCODE_RALT;
    case KeyCode::RightSuper: return SDL_SCANCODE_RGUI;
    default: return SDL_SCANCODE_UNKNOWN;
    }
}

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
        case SDL_EVENT_MOUSE_WHEEL: _mouseState.wheel += static_cast<i32>(e.wheel.y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            const bool down = (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            const int btn = static_cast<int>(e.button.button);
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
        const auto sc = MapKeyCodeToSDLScancode(key);
        if (static_cast<Size>(sc) >= kKeyCount) { return false; }
        return _keysCurrent[sc];
    }

    XIHE_NODISCARD bool wasKeyPressed(KeyCode key) const override
    {
        const auto sc = MapKeyCodeToSDLScancode(key);
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
