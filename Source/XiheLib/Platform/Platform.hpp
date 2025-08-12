/**
 * @File Platform.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <string_view>

#include "Core/Base/Defines.hpp"
#include "Platform/Window.hpp"

namespace xihe {
class XIHE_API IPlatform
{
public:
    virtual ~IPlatform() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    virtual std::unique_ptr<IWindow> createWindow(const WindowDesc& desc) = 0;

    // 基础设施
    XIHE_NODISCARD virtual double timeSeconds() const = 0;
    XIHE_NODISCARD virtual const char* appDataPath() const = 0;

    // 实用工具（最小实现）
    XIHE_NODISCARD virtual const char* clipboardText() const = 0;
    virtual bool setClipboardText(const char* text) = 0;
    virtual bool showMessageBox(const char* title, const char* message) = 0;
};
} // namespace xihe
