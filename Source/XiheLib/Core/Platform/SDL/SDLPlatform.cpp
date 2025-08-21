/**
 * @File SDLPlatform.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/22
 * @Brief This file is part of Xihe.
 */

#include "../Platform.hpp"
#include "../Window.hpp"
#include "Core/Base/Log.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
#include <SDL3/SDL.h>
XIHE_POP_WARNING

namespace xihe {
// 前向声明
std::unique_ptr<Window> CreateSDLWindow(const WindowDesc& desc);

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
    IInput* getInput() override
    {
        // TODO: 实现输入系统
        return nullptr;
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
