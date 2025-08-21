/**
 * @File SDLPlatform.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#include "../Platform.hpp"
#include "../Window.hpp"
#include "../Input.hpp"
#include "Core/Base/Log.hpp"
#include "Core/Events/Event.hpp"

#include <unordered_map>

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

using namespace xihe;

namespace {
KeyCode MapSDLScancodeToKeyCode(SDL_Scancode scancode)
{
    // clang-format off
    XIHE_PUSH_WARNING
    XIHE_CLANG_DISABLE_WARNING("-Wswitch-enum")
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
    case SDL_SCANCODE_0: return KeyCode::Num0;
    case SDL_SCANCODE_1: return KeyCode::Num1;
    case SDL_SCANCODE_2: return KeyCode::Num2;
    case SDL_SCANCODE_3: return KeyCode::Num3;
    case SDL_SCANCODE_4: return KeyCode::Num4;
    case SDL_SCANCODE_5: return KeyCode::Num5;
    case SDL_SCANCODE_6: return KeyCode::Num6;
    case SDL_SCANCODE_7: return KeyCode::Num7;
    case SDL_SCANCODE_8: return KeyCode::Num8;
    case SDL_SCANCODE_9: return KeyCode::Num9;
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
    case SDL_SCANCODE_LEFT: return KeyCode::Left;
    case SDL_SCANCODE_RIGHT: return KeyCode::Right;
    case SDL_SCANCODE_UP: return KeyCode::Up;
    case SDL_SCANCODE_DOWN: return KeyCode::Down;
    case SDL_SCANCODE_SPACE: return KeyCode::Space;
    case SDL_SCANCODE_RETURN: return KeyCode::Enter;
    case SDL_SCANCODE_ESCAPE: return KeyCode::Escape;
    case SDL_SCANCODE_TAB: return KeyCode::Tab;
    case SDL_SCANCODE_BACKSPACE: return KeyCode::Backspace;
    case SDL_SCANCODE_DELETE: return KeyCode::Delete;
    case SDL_SCANCODE_LSHIFT: return KeyCode::LeftShift;
    case SDL_SCANCODE_RSHIFT: return KeyCode::RightShift;
    case SDL_SCANCODE_LCTRL: return KeyCode::LeftCtrl;
    case SDL_SCANCODE_RCTRL: return KeyCode::RightCtrl;
    case SDL_SCANCODE_LALT: return KeyCode::LeftAlt;
    case SDL_SCANCODE_RALT: return KeyCode::RightAlt;
    case SDL_SCANCODE_LGUI: return KeyCode::LeftSuper;
    case SDL_SCANCODE_RGUI: return KeyCode::RightSuper;
        
    case SDL_SCANCODE_APOSTROPHE: return KeyCode::Apostrophe;
    case SDL_SCANCODE_COMMA: return KeyCode::Comma;
    case SDL_SCANCODE_MINUS: return KeyCode::Minus;
    case SDL_SCANCODE_PERIOD: return KeyCode::Period;
    case SDL_SCANCODE_SLASH: return KeyCode::Slash;
    case SDL_SCANCODE_BACKSLASH: return KeyCode::Backslash;
    case SDL_SCANCODE_SEMICOLON: return KeyCode::Semicolon;
    case SDL_SCANCODE_EQUALS: return KeyCode::Equals;
    case SDL_SCANCODE_LEFTBRACKET: return KeyCode::LeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET: return KeyCode::RightBracket;
    case SDL_SCANCODE_GRAVE: return KeyCode::Grave;
        
        
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
    case SDL_SCANCODE_KP_PERIOD: return KeyCode::KeypadPeriod;
    case SDL_SCANCODE_KP_DIVIDE: return KeyCode::KeypadDivide;
    case SDL_SCANCODE_KP_MULTIPLY: return KeyCode::KeypadMultiply;
    case SDL_SCANCODE_KP_MINUS: return KeyCode::KeypadMinus;
    case SDL_SCANCODE_KP_PLUS: return KeyCode::KeypadPlus;
    case SDL_SCANCODE_KP_ENTER: return KeyCode::KeypadEnter;
    case SDL_SCANCODE_KP_EQUALS: return KeyCode::KeypadEquals;

    case SDL_SCANCODE_HOME: return KeyCode::Home;
    case SDL_SCANCODE_END: return KeyCode::End;
    case SDL_SCANCODE_PAGEUP: return KeyCode::PageUp;
    case SDL_SCANCODE_PAGEDOWN: return KeyCode::PageDown;
    case SDL_SCANCODE_INSERT: return KeyCode::Insert;
    case SDL_SCANCODE_CAPSLOCK: return KeyCode::CapsLock;
    case SDL_SCANCODE_SCROLLLOCK: return KeyCode::ScrollLock;
    case SDL_SCANCODE_NUMLOCKCLEAR: return KeyCode::NumLock;
    case SDL_SCANCODE_PRINTSCREEN: return KeyCode::PrintScreen;
    case SDL_SCANCODE_PAUSE: return KeyCode::Pause;
    case SDL_SCANCODE_MENU: return KeyCode::Menu;
        
    default: return KeyCode::Unknown;
    }
    XIHE_POP_WARNING
    // clang-format on
}

MouseButton MapSDLMouseButton(u8 sdlButton)
{
    switch (sdlButton)
    {
    case SDL_BUTTON_LEFT:
        return MouseButton::Left;
    case SDL_BUTTON_RIGHT:
        return MouseButton::Right;
    case SDL_BUTTON_MIDDLE:
        return MouseButton::Middle;
    case SDL_BUTTON_X1:
        return MouseButton::X1;
    case SDL_BUTTON_X2:
        return MouseButton::X2;
    default:
        return MouseButton::Left;
    }
}

Event MapSDLEventToXiheEvent(const SDL_Event& sdlEvent)
{
    Event xiheEvent{};
    switch (sdlEvent.type)
    {
    case SDL_EVENT_QUIT:
    {
        xiheEvent.header.type     = EventType::WindowCloseRequested;
        xiheEvent.header.category = EventCategory::Window;
        xiheEvent.payload         = WindowCloseRequestedEvent{};
        break;
    }

    case SDL_EVENT_WINDOW_RESIZED:
    {
        xiheEvent.header.type     = EventType::WindowResize;
        xiheEvent.header.category = EventCategory::Window;
        xiheEvent.payload         = WindowResizeEvent{
                static_cast<u32>(sdlEvent.window.data1),
                static_cast<u32>(sdlEvent.window.data2)
        };
        break;
    }

    case SDL_EVENT_KEY_DOWN:
    {
        KeyCode key = MapSDLScancodeToKeyCode(sdlEvent.key.scancode);
        if (key != KeyCode::Unknown)
        {
            xiheEvent.header.type     = EventType::KeyDown;
            xiheEvent.header.category = EventCategory::Input;
            xiheEvent.payload         = KeyDownEvent{key};
        }
        break;
    }

    case SDL_EVENT_KEY_UP:
    {
        KeyCode key = MapSDLScancodeToKeyCode(sdlEvent.key.scancode);
        if (key != KeyCode::Unknown)
        {
            xiheEvent.header.type     = EventType::KeyUp;
            xiheEvent.header.category = EventCategory::Input;
            xiheEvent.payload         = KeyUpEvent{key};
        }
        break;
    }

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    {
        MouseButton button        = MapSDLMouseButton(sdlEvent.button.button);
        xiheEvent.header.type     = EventType::MouseDown;
        xiheEvent.header.category = EventCategory::Input;
        xiheEvent.payload         = MouseDownEvent{
                button,
                static_cast<i32>(sdlEvent.button.x),
                static_cast<i32>(sdlEvent.button.y)
        };
        break;
    }

    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        MouseButton button        = MapSDLMouseButton(sdlEvent.button.button);
        xiheEvent.header.type     = EventType::MouseUp;
        xiheEvent.header.category = EventCategory::Input;
        xiheEvent.payload         = MouseUpEvent{
                button,
                static_cast<i32>(sdlEvent.button.x),
                static_cast<i32>(sdlEvent.button.y)
        };
        break;
    }

    case SDL_EVENT_MOUSE_MOTION:
    {
        xiheEvent.header.type     = EventType::MouseMove;
        xiheEvent.header.category = EventCategory::Input;
        xiheEvent.payload         = MouseMoveEvent{
                static_cast<i32>(sdlEvent.motion.x),
                static_cast<i32>(sdlEvent.motion.y),
                static_cast<i32>(sdlEvent.motion.xrel),
                static_cast<i32>(sdlEvent.motion.yrel)
        };
        break;
    }

    case SDL_EVENT_MOUSE_WHEEL:
    {
        xiheEvent.header.type = EventType::MouseWheel;
        xiheEvent.header.category = EventCategory::Input;
        xiheEvent.payload = MouseWheelEvent{static_cast<f32>(sdlEvent.wheel.x), static_cast<f32>(sdlEvent.wheel.y), 0, 0};
        break;
    }

    case SDL_EVENT_TEXT_INPUT:
    {
        xiheEvent.header.type     = EventType::TextInput;
        xiheEvent.header.category = EventCategory::Input;
        xiheEvent.payload         = TextInputEvent{static_cast<u32>(static_cast<unsigned char>(sdlEvent.text.text[0]))};
        break;
    }

    default:
        // ....
        break;
    }

    return xiheEvent;
}
} // namespace 

namespace xihe {
// 前向声明
std::unique_ptr<Window> CreateSDLWindow(const WindowDesc& desc);
std::unique_ptr<Input> CreateSDLInput();


class SDLPlatform : public Platform
{
public:
    SDLPlatform()           = default;
    ~SDLPlatform() override = default;

    // 核心平台管理
    bool initialize() override
    {
        if (_initialized)
        {
            return true;
        }

        // 初始化SDL3核心子系统
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        {
            XIHE_CORE_ERROR("SDL - 初始化失败: {}", SDL_GetError());
            return false;
        }

        _initialized = true;
        XIHE_CORE_INFO("Platform 初始化成功");
        return true;
    }

    void shutdown() override
    {
        if (!_initialized)
        {
            return;
        }

        SDL_Quit();
        _initialized = false;
        XIHE_CORE_INFO("Platform 已完全关闭");
    }

    // 窗口管理
    std::unique_ptr<Window> createWindow(const WindowDesc& desc) override
    {
        if (!_initialized)
        {
            XIHE_CORE_ERROR("无法创建窗口: 平台对象没有初始化");
            return nullptr;
        }

        return CreateSDLWindow(desc);
    }

    // 输入系统 - 暂时空实现
    Input* getInput() override
    {
        // TODO: 实现输入系统
        return nullptr;
    }

    // 事件轮询
    bool pollEvent(Event& event) override
    {
        if (!_initialized)
        {
            return false;
        }

        SDL_Event sdlEvent;
        if (!SDL_PollEvent(&sdlEvent))
        {
            return false;
        }

        event = MapSDLEventToXiheEvent(sdlEvent);
        return event.header.category != EventCategory::None;
    }

    // 时间查询
    double timeSeconds() const override
    {
        if (!_initialized)
        {
            return 0.0;
        }
        // SDL3使用纳秒精度，转换为秒
        return static_cast<double>(SDL_GetTicksNS()) / 1000000000.0;
    }

    // 文件系统 - 暂时空实现
    std::string appDataPath() const override
    {
        // TODO: 实现应用数据路径获取
        return "";
    }

    // 剪贴板操作 - 暂时空实现
    std::string clipboardText() const override
    {
        // TODO: 实现剪贴板读取
        return "";
    }

    bool setClipboardText(std::string_view text) override
    {
        // TODO: 实现剪贴板写入
        return false;
    }

    // 系统对话框 - 暂时空实现
    bool showMessageBox(std::string_view title, std::string_view message, int type) override
    {
        // TODO: 实现消息框显示
        return false;
    }

    // 系统信息 - 暂时空实现
    std::string platformName() const override
    {
        // TODO: 实现平台名称获取
        return "Unknown";
    }

    std::string cpuInfo() const override
    {
        // TODO: 实现CPU信息获取
        return "Unknown";
    }

    u32 ramMB() const override
    {
        // TODO: 实现内存信息获取
        return 0;
    }

private:
    bool _initialized = false;
};

// 工厂函数实现
std::unique_ptr<Platform> CreatePlatform()
{
    return std::make_unique<SDLPlatform>();
}
} // namespace xihe
