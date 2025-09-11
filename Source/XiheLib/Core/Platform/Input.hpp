/**
 * @File Input.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <string>
#include "Core/Base/Defines.hpp"

namespace xihe {
// 键盘按键枚举
enum class KeyCode : u32
{
    Unknown = 0,

    // 字母键
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // 数字键
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // 功能键
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // 方向键
    Left, Right, Up, Down,

    // 控制键
    Space, Enter, Escape, Tab, Backspace, Delete,
    LeftShift, RightShift, LeftCtrl, RightCtrl, LeftAlt, RightAlt, LeftSuper, RightSuper,

    // 符号键
    Apostrophe, Comma, Minus, Period, Slash, Backslash, Semicolon, Equals, LeftBracket, RightBracket, Grave,

    // 小键盘
    Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9, KeypadPeriod, KeypadDivide,
    KeypadMultiply, KeypadMinus, KeypadPlus, KeypadEnter, KeypadEquals,

    // 其他常用键
    Home, End, PageUp, PageDown, Insert, CapsLock, ScrollLock, NumLock, PrintScreen, Pause, Menu,

    Count
};

// 鼠标按钮枚举
enum class MouseButton : u32
{
    Left = 0,
    Right,
    Middle,
    X1,
    X2,

    Count
};

// 鼠标状态结构
struct MouseState
{
    i32 x{0};
    i32 y{0};
    i32 deltaX{0};
    i32 deltaY{0};
    f32 wheelX{0.0f};
    f32 wheelY{0.0f};
    bool buttons[static_cast<u32>(MouseButton::Count)]{false};
};

// IInput 抽象接口
class XIHE_API Input
{
public:
    virtual ~Input() = default;

    // 键盘状态查询
    XIHE_NODISCARD virtual bool isKeyDown(KeyCode key) const = 0;
    XIHE_NODISCARD virtual bool wasKeyPressed(KeyCode key) const = 0;  // 本帧按下
    XIHE_NODISCARD virtual bool wasKeyReleased(KeyCode key) const = 0; // 本帧释放

    // 鼠标状态查询
    XIHE_NODISCARD virtual MouseState getMouseState() const = 0;
    XIHE_NODISCARD virtual bool isMouseButtonDown(MouseButton button) const = 0;
    XIHE_NODISCARD virtual bool wasMouseButtonPressed(MouseButton button) const = 0;  // 本帧按下
    XIHE_NODISCARD virtual bool wasMouseButtonReleased(MouseButton button) const = 0; // 本帧释放

    // 文本输入
    XIHE_NODISCARD virtual std::string getTextInput() const = 0;
    virtual void clearTextInput() = 0;

    // 更新输入状态（每帧调用）
    virtual void update() = 0;
};
} // namespace xihe
