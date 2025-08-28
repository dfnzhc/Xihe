/**
 * @File IPlatform.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "Core/Base/Defines.hpp"
#include "Core/Platform/Window.hpp"
#include "Core/Events/Event.hpp"

namespace xihe {
class Input;
class XIHE_API Platform
{
public:
    virtual ~Platform() = default;

    // =============================
    // 核心功能
    // =============================

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;

    // =============================
    // 窗口管理
    // =============================

    virtual std::unique_ptr<Window> createWindow(const WindowDesc& desc) = 0;

    // =============================
    // 输入系统
    // =============================

    virtual Input* getInput() = 0;

    // =============================
    // 事件系统
    // =============================

    virtual bool pollEvent(IEvent& event) = 0;

    // =============================
    // 时间
    // =============================

    virtual double timeSeconds() const = 0;

    // =============================
    // 文件系统
    // =============================

    virtual std::string appDataPath() const = 0;

    // =============================
    // 剪切板
    // =============================

    virtual std::string clipboardText() const = 0;

    virtual bool setClipboardText(std::string_view text) = 0;

    // =============================
    // 对话框
    // =============================

    virtual bool showMessageBox(std::string_view title, std::string_view message, int type = 0) = 0;

    // =============================
    // 系统信息
    // =============================

    virtual std::string platformName() const = 0;
    virtual std::string cpuInfo() const = 0;
    virtual u32 ramMB() const = 0;
};

XIHE_API std::unique_ptr<Platform> CreatePlatform();
} // namespace xihe
