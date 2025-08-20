/**
 * @File SDLInput.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#include <array>
#include <atomic>

#include "Core/Base/Defines.hpp"
#include "Core/Context.hpp"
#include "Platform/Input.hpp"
#include "Core/Events/EventBus.hpp"
#include "Core/Utils/Time/Clock.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

using namespace xihe;

namespace {
constexpr Size kKeyCount = 512;

SDL_Scancode MapKeyCodeToSDLScancode(KeyCode key)
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

    XIHE_PUSH_WARNING
    XIHE_CLANG_DISABLE_WARNING("-Wswitch-enum")
    switch (key) {
    case KeyCode::Space:        return SDL_SCANCODE_SPACE;
    case KeyCode::Apostrophe:   return SDL_SCANCODE_APOSTROPHE;
    case KeyCode::Comma:        return SDL_SCANCODE_COMMA;
    case KeyCode::Minus:        return SDL_SCANCODE_MINUS;
    case KeyCode::Period:       return SDL_SCANCODE_PERIOD;
    case KeyCode::Slash:        return SDL_SCANCODE_SLASH;
    case KeyCode::Semicolon:    return SDL_SCANCODE_SEMICOLON;
    case KeyCode::Equal:        return SDL_SCANCODE_EQUALS;
    case KeyCode::LeftBracket:  return SDL_SCANCODE_LEFTBRACKET;
    case KeyCode::Backslash:    return SDL_SCANCODE_BACKSLASH;
    case KeyCode::RightBracket: return SDL_SCANCODE_RIGHTBRACKET;
    case KeyCode::GraveAccent:  return SDL_SCANCODE_GRAVE;

    case KeyCode::Escape:       return SDL_SCANCODE_ESCAPE;
    case KeyCode::Tab:          return SDL_SCANCODE_TAB;
    case KeyCode::Enter:        return SDL_SCANCODE_RETURN;
    case KeyCode::Backspace:    return SDL_SCANCODE_BACKSPACE;
    case KeyCode::Insert:       return SDL_SCANCODE_INSERT;
    case KeyCode::Del:          return SDL_SCANCODE_DELETE;
    case KeyCode::Right:        return SDL_SCANCODE_RIGHT;
    case KeyCode::Left:         return SDL_SCANCODE_LEFT;
    case KeyCode::Down:         return SDL_SCANCODE_DOWN;
    case KeyCode::Up:           return SDL_SCANCODE_UP;
    case KeyCode::PageUp:       return SDL_SCANCODE_PAGEUP;
    case KeyCode::PageDown:     return SDL_SCANCODE_PAGEDOWN;
    case KeyCode::Home:         return SDL_SCANCODE_HOME;
    case KeyCode::End:          return SDL_SCANCODE_END;
    case KeyCode::CapsLock:     return SDL_SCANCODE_CAPSLOCK;
    case KeyCode::ScrollLock:   return SDL_SCANCODE_SCROLLLOCK;
    case KeyCode::NumLock:      return SDL_SCANCODE_NUMLOCKCLEAR;
    case KeyCode::PrintScreen:  return SDL_SCANCODE_PRINTSCREEN;
    case KeyCode::Pause:        return SDL_SCANCODE_PAUSE;
    case KeyCode::F1:           return SDL_SCANCODE_F1;
    case KeyCode::F2:           return SDL_SCANCODE_F2;
    case KeyCode::F3:           return SDL_SCANCODE_F3;
    case KeyCode::F4:           return SDL_SCANCODE_F4;
    case KeyCode::F5:           return SDL_SCANCODE_F5;
    case KeyCode::F6:           return SDL_SCANCODE_F6;
    case KeyCode::F7:           return SDL_SCANCODE_F7;
    case KeyCode::F8:           return SDL_SCANCODE_F8;
    case KeyCode::F9:           return SDL_SCANCODE_F9;
    case KeyCode::F10:          return SDL_SCANCODE_F10;
    case KeyCode::F11:          return SDL_SCANCODE_F11;
    case KeyCode::F12:          return SDL_SCANCODE_F12;
    case KeyCode::Keypad0:      return SDL_SCANCODE_KP_0;
    case KeyCode::Keypad1:      return SDL_SCANCODE_KP_1;
    case KeyCode::Keypad2:      return SDL_SCANCODE_KP_2;
    case KeyCode::Keypad3:      return SDL_SCANCODE_KP_3;
    case KeyCode::Keypad4:      return SDL_SCANCODE_KP_4;
    case KeyCode::Keypad5:      return SDL_SCANCODE_KP_5;
    case KeyCode::Keypad6:      return SDL_SCANCODE_KP_6;
    case KeyCode::Keypad7:      return SDL_SCANCODE_KP_7;
    case KeyCode::Keypad8:      return SDL_SCANCODE_KP_8;
    case KeyCode::Keypad9:      return SDL_SCANCODE_KP_9;
    case KeyCode::KeypadDel:    return SDL_SCANCODE_KP_PERIOD;
    case KeyCode::KeypadDivide: return SDL_SCANCODE_KP_DIVIDE;
    case KeyCode::KeypadMultiply: return SDL_SCANCODE_KP_MULTIPLY;
    case KeyCode::KeypadSubtract: return SDL_SCANCODE_KP_MINUS;
    case KeyCode::KeypadAdd:    return SDL_SCANCODE_KP_PLUS;
    case KeyCode::KeypadEnter:  return SDL_SCANCODE_KP_ENTER;
    case KeyCode::KeypadEqual:  return SDL_SCANCODE_KP_EQUALS;
    case KeyCode::LeftShift:    return SDL_SCANCODE_LSHIFT;
    case KeyCode::LeftControl:  return SDL_SCANCODE_LCTRL;
    case KeyCode::LeftAlt:      return SDL_SCANCODE_LALT;
    case KeyCode::LeftSuper:    return SDL_SCANCODE_LGUI;
    case KeyCode::RightShift:   return SDL_SCANCODE_RSHIFT;
    case KeyCode::RightControl: return SDL_SCANCODE_RCTRL;
    case KeyCode::RightAlt:     return SDL_SCANCODE_RALT;
    case KeyCode::RightSuper:   return SDL_SCANCODE_RGUI;
    default:                    return SDL_SCANCODE_UNKNOWN;
    }
    XIHE_POP_WARNING
    // clang-format on
}

KeyCode MapSDLScancodeToKeyCode(SDL_Scancode scancode)
{
    // clang-format off
    switch (scancode) {
    case SDL_SCANCODE_A: return KeyCode::A;
    case SDL_SCANCODE_B: return KeyCode::B;
    case SDL_SCANCODE_C: return KeyCode::C;
    case SDL_SCANCODE_D: return KeyCode::D;
    case SDL_SCANCODE_E: return KeyCode::E;
    case SDL_SCANCODE_F: return KeyCode::F;
    case SDL_SCANCODE_G: return KeyCode::G;
    case SDL_SCANCODE_H: return KeyCode::H;
    case SDL_SCANCODE_I: return KeyCode::I;
    case SDL_SCANCODE_J: return KeyCode::J;
    case SDL_SCANCODE_K: return KeyCode::K;
    case SDL_SCANCODE_L: return KeyCode::L;
    case SDL_SCANCODE_M: return KeyCode::M;
    case SDL_SCANCODE_N: return KeyCode::N;
    case SDL_SCANCODE_O: return KeyCode::O;
    case SDL_SCANCODE_P: return KeyCode::P;
    case SDL_SCANCODE_Q: return KeyCode::Q;
    case SDL_SCANCODE_R: return KeyCode::R;
    case SDL_SCANCODE_S: return KeyCode::S;
    case SDL_SCANCODE_T: return KeyCode::T;
    case SDL_SCANCODE_U: return KeyCode::U;
    case SDL_SCANCODE_V: return KeyCode::V;
    case SDL_SCANCODE_W: return KeyCode::W;
    case SDL_SCANCODE_X: return KeyCode::X;
    case SDL_SCANCODE_Y: return KeyCode::Y;
    case SDL_SCANCODE_Z: return KeyCode::Z;
    case SDL_SCANCODE_0: return KeyCode::Key0;
    case SDL_SCANCODE_1: return KeyCode::Key1;
    case SDL_SCANCODE_2: return KeyCode::Key2;
    case SDL_SCANCODE_3: return KeyCode::Key3;
    case SDL_SCANCODE_4: return KeyCode::Key4;
    case SDL_SCANCODE_5: return KeyCode::Key5;
    case SDL_SCANCODE_6: return KeyCode::Key6;
    case SDL_SCANCODE_7: return KeyCode::Key7;
    case SDL_SCANCODE_8: return KeyCode::Key8;
    case SDL_SCANCODE_9: return KeyCode::Key9;
    case SDL_SCANCODE_SPACE: return KeyCode::Space;
    case SDL_SCANCODE_APOSTROPHE: return KeyCode::Apostrophe;
    case SDL_SCANCODE_COMMA: return KeyCode::Comma;
    case SDL_SCANCODE_MINUS: return KeyCode::Minus;
    case SDL_SCANCODE_PERIOD: return KeyCode::Period;
    case SDL_SCANCODE_SLASH: return KeyCode::Slash;
    case SDL_SCANCODE_SEMICOLON: return KeyCode::Semicolon;
    case SDL_SCANCODE_EQUALS: return KeyCode::Equal;
    case SDL_SCANCODE_LEFTBRACKET: return KeyCode::LeftBracket;
    case SDL_SCANCODE_BACKSLASH: return KeyCode::Backslash;
    case SDL_SCANCODE_RIGHTBRACKET: return KeyCode::RightBracket;
    case SDL_SCANCODE_GRAVE: return KeyCode::GraveAccent;
    case SDL_SCANCODE_ESCAPE: return KeyCode::Escape;
    case SDL_SCANCODE_TAB: return KeyCode::Tab;
    case SDL_SCANCODE_RETURN: return KeyCode::Enter;
    case SDL_SCANCODE_BACKSPACE: return KeyCode::Backspace;
    case SDL_SCANCODE_INSERT: return KeyCode::Insert;
    case SDL_SCANCODE_DELETE: return KeyCode::Del;
    case SDL_SCANCODE_RIGHT: return KeyCode::Right;
    case SDL_SCANCODE_LEFT: return KeyCode::Left;
    case SDL_SCANCODE_DOWN: return KeyCode::Down;
    case SDL_SCANCODE_UP: return KeyCode::Up;
    case SDL_SCANCODE_PAGEUP: return KeyCode::PageUp;
    case SDL_SCANCODE_PAGEDOWN: return KeyCode::PageDown;
    case SDL_SCANCODE_HOME: return KeyCode::Home;
    case SDL_SCANCODE_END: return KeyCode::End;
    case SDL_SCANCODE_CAPSLOCK: return KeyCode::CapsLock;
    case SDL_SCANCODE_SCROLLLOCK: return KeyCode::ScrollLock;
    case SDL_SCANCODE_NUMLOCKCLEAR: return KeyCode::NumLock;
    case SDL_SCANCODE_PRINTSCREEN: return KeyCode::PrintScreen;
    case SDL_SCANCODE_PAUSE: return KeyCode::Pause;
    case SDL_SCANCODE_F1: return KeyCode::F1;
    case SDL_SCANCODE_F2: return KeyCode::F2;
    case SDL_SCANCODE_F3: return KeyCode::F3;
    case SDL_SCANCODE_F4: return KeyCode::F4;
    case SDL_SCANCODE_F5: return KeyCode::F5;
    case SDL_SCANCODE_F6: return KeyCode::F6;
    case SDL_SCANCODE_F7: return KeyCode::F7;
    case SDL_SCANCODE_F8: return KeyCode::F8;
    case SDL_SCANCODE_F9: return KeyCode::F9;
    case SDL_SCANCODE_F10: return KeyCode::F10;
    case SDL_SCANCODE_F11: return KeyCode::F11;
    case SDL_SCANCODE_F12: return KeyCode::F12;
    case SDL_SCANCODE_KP_0: return KeyCode::Keypad0;
    case SDL_SCANCODE_KP_1: return KeyCode::Keypad1;
    case SDL_SCANCODE_KP_2: return KeyCode::Keypad2;
    case SDL_SCANCODE_KP_3: return KeyCode::Keypad3;
    case SDL_SCANCODE_KP_4: return KeyCode::Keypad4;
    case SDL_SCANCODE_KP_5: return KeyCode::Keypad5;
    case SDL_SCANCODE_KP_6: return KeyCode::Keypad6;
    case SDL_SCANCODE_KP_7: return KeyCode::Keypad7;
    case SDL_SCANCODE_KP_8: return KeyCode::Keypad8;
    case SDL_SCANCODE_KP_9: return KeyCode::Keypad9;
    case SDL_SCANCODE_KP_PERIOD: return KeyCode::KeypadDel;
    case SDL_SCANCODE_KP_DIVIDE: return KeyCode::KeypadDivide;
    case SDL_SCANCODE_KP_MULTIPLY: return KeyCode::KeypadMultiply;
    case SDL_SCANCODE_KP_MINUS: return KeyCode::KeypadSubtract;
    case SDL_SCANCODE_KP_PLUS: return KeyCode::KeypadAdd;
    case SDL_SCANCODE_KP_ENTER: return KeyCode::KeypadEnter;
    case SDL_SCANCODE_KP_EQUALS: return KeyCode::KeypadEqual;
    case SDL_SCANCODE_LSHIFT: return KeyCode::LeftShift;
    case SDL_SCANCODE_LCTRL: return KeyCode::LeftControl;
    case SDL_SCANCODE_LALT: return KeyCode::LeftAlt;
    case SDL_SCANCODE_LGUI: return KeyCode::LeftSuper;
    case SDL_SCANCODE_RSHIFT: return KeyCode::RightShift;
    case SDL_SCANCODE_RCTRL: return KeyCode::RightControl;
    case SDL_SCANCODE_RALT: return KeyCode::RightAlt;
    case SDL_SCANCODE_RGUI: return KeyCode::RightSuper;
    case SDL_SCANCODE_MENU: return KeyCode::Menu;
    default: return KeyCode::Unknown;
    }
    // clang-format on
}

Event MakeCoreEventFromSDL(const SDL_Event& e)
{
    Event ev;
    ev.header.timestamp = Clock::now();
    switch (e.type)
    {
    case SDL_EVENT_TEXT_INPUT:
    {
        if (e.text.text[0] != '\0')
        {
            ev.header.type     = EventType::TextInput;
            ev.header.category = EventCategory::Input;
            ev.payload         = TextInputEvent{static_cast<u32>(static_cast<unsigned char>(e.text.text[0]))};
        }
    }
    break;
    case SDL_EVENT_KEY_DOWN:
    {
        ev.header.type     = EventType::KeyDown;
        ev.header.category = EventCategory::Input;
        ev.payload         = KeyDownEvent{MapSDLScancodeToKeyCode(e.key.scancode), e.key.repeat != 0};
        break;
    }
    case SDL_EVENT_KEY_UP:
    {
        ev.header.type     = EventType::KeyUp;
        ev.header.category = EventCategory::Input;
        ev.payload         = KeyUpEvent{MapSDLScancodeToKeyCode(e.key.scancode)};
        break;
    }
    case SDL_EVENT_MOUSE_MOTION:
    {
        ev.header.type     = EventType::MouseMove;
        ev.header.category = EventCategory::Input;
        ev.payload         = MouseMoveEvent{static_cast<i32>(e.motion.x), static_cast<i32>(e.motion.y),
                                            static_cast<i32>(e.motion.xrel),
                                            static_cast<i32>(e.motion.yrel)};
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        const bool down       = (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
        const MouseButton btn = (e.button.button == SDL_BUTTON_LEFT)
                ? MouseButton::Left
                : (e.button.button == SDL_BUTTON_RIGHT)
                ? MouseButton::Right
                : (e.button.button == SDL_BUTTON_MIDDLE)
                ? MouseButton::Middle
                : MouseButton::Left;
        if (down)
        {
            ev.header.type = EventType::MouseDown;
            ev.payload = MouseDownEvent{btn, static_cast<i32>(e.button.x), static_cast<i32>(e.button.y), e.button.clicks};
        }
        else
        {
            ev.header.type = EventType::MouseUp;
            ev.payload     = MouseUpEvent{btn, static_cast<i32>(e.button.x), static_cast<i32>(e.button.y)};
        }
        ev.header.category = EventCategory::Input;
        break;
    }
    case SDL_EVENT_MOUSE_WHEEL:
    {
        ev.header.type     = EventType::MouseWheel;
        ev.header.category = EventCategory::Input;
        ev.payload         = MouseWheelEvent{static_cast<f32>(e.wheel.x), static_cast<f32>(e.wheel.y), 0, 0};
        break;
    }
    default:
        break;
    }

    return ev;
}
} // namespace

namespace xihe {
std::optional<Event> SDLEventToInputEvent(const SDL_Event& e)
{
    auto coreEvent = MakeCoreEventFromSDL(e);
    if (coreEvent.header.category != EventCategory::None)
    {
        return coreEvent;
    }
    return std::nullopt;
}
} // namespace xihe
