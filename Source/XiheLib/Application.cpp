/**
 * @File Application.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */
#include "Application.hpp"

#include "Core/Context.hpp"
#include "Core/Base/Log.hpp"
#include "Platform/Platform.hpp"
#include "Platform/PlatformFactory.hpp"

using namespace xihe;

bool Application::run()
{
    if (!Context::Create()) { return false; }

    XIHE_CORE_INFO("羲和引擎启动中...");

    // 创建平台（SDL 实现）
    _platform = CreatePlatformSDL();
    if (!_platform || !_platform->initialize()) {
        XIHE_CORE_ERROR("平台层初始化失败");
        onShutdown();
        Context::Destroy();
        return false;
    }

    _running = onInit();
    while (_running.load(std::memory_order_relaxed)) { onTick(); }

    onShutdown();

    if (_platform) {
        _platform->shutdown();
        _platform.reset();
    }

    XIHE_CORE_INFO("羲和引擎正在退出");
    Context::Destroy();
    return true;
}
