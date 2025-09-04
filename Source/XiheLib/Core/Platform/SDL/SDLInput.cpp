/**
 * @File SDLInput.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#include "../Input.hpp"
#include "Core/Utils/Logger.hpp"
#include "Core/Events/Event.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
class SDLInput : public Input
{
public:
    SDLInput()           = default;
    ~SDLInput() override = default;

    // 键盘状态查询
    bool isKeyDown(KeyCode key) const override
    {
        auto it = _currentKeyStates.find(key);
        return it != _currentKeyStates.end() && it->second;
    }

    bool wasKeyPressed(KeyCode key) const override
    {
        return _pressedThisFrame.find(key) != _pressedThisFrame.end();
    }

    bool wasKeyReleased(KeyCode key) const override
    {
        return _releasedThisFrame.find(key) != _releasedThisFrame.end();
    }

    // 鼠标状态查询
    MouseState getMouseState() const override
    {
        return _mouseState;
    }

    bool isMouseButtonDown(MouseButton button) const override
    {
        auto it = _currentMouseStates.find(button);
        return it != _currentMouseStates.end() && it->second;
    }

    bool wasMouseButtonPressed(MouseButton button) const override
    {
        return _mouseButtonsPressedThisFrame.find(button) != _mouseButtonsPressedThisFrame.end();
    }

    bool wasMouseButtonReleased(MouseButton button) const override
    {
        return _mouseButtonsReleasedThisFrame.find(button) != _mouseButtonsReleasedThisFrame.end();
    }

    // 文本输入
    std::string getTextInput() const override
    {
        return _textInputBuffer;
    }

    void clearTextInput() override
    {
        _textInputBuffer.clear();
    }

    // 每帧更新
    void update() override
    {
        // 清理本帧触发的事件
        _pressedThisFrame.clear();
        _releasedThisFrame.clear();
        _mouseButtonsPressedThisFrame.clear();
        _mouseButtonsReleasedThisFrame.clear();

        // 更新鼠标位置
        updateMouseState();
    }

    // 内部方法：处理事件（由平台层调用）
    void processEvent(const IEvent& event)
    {
#if 0
        switch (event.header.type)
        {
        case EventType::KeyDown:
            if (auto* keyEvent = std::get_if<KeyDownEvent>(&event.payload))
            {
                processKeyEvent(keyEvent->key, true);
            }
            break;
        case EventType::KeyUp:
            if (auto* keyEvent = std::get_if<KeyUpEvent>(&event.payload))
            {
                processKeyEvent(keyEvent->key, false);
            }
            break;
        case EventType::MouseDown:
            if (auto* mouseEvent = std::get_if<MouseDownEvent>(&event.payload))
            {
                processMouseButtonEvent(mouseEvent->button, true, mouseEvent->x, mouseEvent->y);
            }
            break;
        case EventType::MouseUp:
            if (auto* mouseEvent = std::get_if<MouseUpEvent>(&event.payload))
            {
                processMouseButtonEvent(mouseEvent->button, false, mouseEvent->x, mouseEvent->y);
            }
            break;
        case EventType::MouseMove:
            if (auto* mouseEvent = std::get_if<MouseMoveEvent>(&event.payload))
            {
                processMouseMoveEvent(mouseEvent->x, mouseEvent->y, mouseEvent->deltaX, mouseEvent->deltaY);
            }
            break;
        case EventType::MouseWheel:
            if (auto* wheelEvent = std::get_if<MouseWheelEvent>(&event.payload))
            {
                processMouseWheelEvent(static_cast<i32>(wheelEvent->deltaX), static_cast<i32>(wheelEvent->deltaY));
            }
            break;
        case EventType::TextInput:
            if (auto* textEvent = std::get_if<TextInputEvent>(&event.payload))
            {
                // 将字符转换为字符串
                char textChar = static_cast<char>(textEvent->character);
                if (textChar > 0) // 确保是有效字符
                {
                    processTextInputEvent(std::string(1, textChar));
                }
            }
            break;
        default:
            break;
        }
#endif
    }

    void processKeyEvent(KeyCode key, bool pressed)
    {
        if (pressed)
        {
            if (_currentKeyStates[key] == false)
            {
                _pressedThisFrame.insert(key);
            }
            _currentKeyStates[key] = true;
        }
        else
        {
            if (_currentKeyStates[key] == true)
            {
                _releasedThisFrame.insert(key);
            }
            _currentKeyStates[key] = false;
        }
    }

    void processMouseButtonEvent(MouseButton button, bool pressed, i32 x, i32 y)
    {
        if (pressed)
        {
            if (_currentMouseStates[button] == false)
            {
                _mouseButtonsPressedThisFrame.insert(button);
            }
            _currentMouseStates[button] = true;
        }
        else
        {
            if (_currentMouseStates[button] == true)
            {
                _mouseButtonsReleasedThisFrame.insert(button);
            }
            _currentMouseStates[button] = false;
        }

        _mouseState.x = x;
        _mouseState.y = y;
    }

    void processMouseMoveEvent(i32 x, i32 y, i32 deltaX, i32 deltaY)
    {
        _mouseState.x      = x;
        _mouseState.y      = y;
        _mouseState.deltaX = deltaX;
        _mouseState.deltaY = deltaY;
    }

    void processMouseWheelEvent(i32 deltaX, i32 deltaY)
    {
        _mouseState.deltaX = deltaX;
        _mouseState.deltaY = deltaY;
    }

    void processTextInputEvent(const std::string& text)
    {
        _textInputBuffer += text;
    }

private:
    // 键盘状态
    std::unordered_map<KeyCode, bool> _currentKeyStates;
    std::unordered_set<KeyCode> _pressedThisFrame;
    std::unordered_set<KeyCode> _releasedThisFrame;

    // 鼠标状态
    MouseState _mouseState{};
    std::unordered_map<MouseButton, bool> _currentMouseStates;
    std::unordered_set<MouseButton> _mouseButtonsPressedThisFrame;
    std::unordered_set<MouseButton> _mouseButtonsReleasedThisFrame;

    // 文本输入缓冲
    std::string _textInputBuffer;

    void updateMouseState()
    {
        // 重置滚轮增量（每帧重置）
        _mouseState.deltaX = 0;
        _mouseState.deltaY = 0;
        _mouseState.deltaX = 0;
        _mouseState.deltaY = 0;

        // 获取当前鼠标位置
        float x, y;
        if (SDL_GetMouseState(&x, &y))
        {
            _mouseState.x = static_cast<i32>(x);
            _mouseState.y = static_cast<i32>(y);
        }
    }
};

// 工厂函数实现
std::unique_ptr<Input> CreateSDLInput()
{
    return std::make_unique<SDLInput>();
}
} // namespace xihe
