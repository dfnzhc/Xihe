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
#include "Renderer/Renderer.hpp"

using namespace xihe;

bool Application::run()
{
    if (!Context::Create())
    {
        return false;
    }

    XIHE_CORE_INFO("羲和引擎启动中...");

    // 创建 SDL 平台实现
    _platform = CreatePlatform();
    if (!_platform || !_platform->initialize())
    {
        XIHE_CORE_ERROR("平台层初始化失败");
        onShutdown();
        Context::Destroy();
        return false;
    }

    // 创建高层渲染器
    _renderer = CreateRenderer(Renderer::Backend::Auto);
    if (!_renderer || !_renderer->initialize(_platform.get()))
    {
        XIHE_CORE_ERROR("渲染器初始化失败");
        onShutdown();
        if (_platform)
        {
            _platform->shutdown();
            _platform.reset();
        }
        Context::Destroy();
        return false;
    }

    _running        = onInit();
    double lastTime = _platform->timeSeconds();
    while (_running.load(std::memory_order_relaxed))
    {
        const double now = _platform->timeSeconds();
        const double dt  = now - lastTime;
        lastTime         = now;
        _renderer->beginFrame(dt);
        onTick();
        _renderer->render();
        _renderer->endFrame();
    }

    onShutdown();

    if (_renderer)
    {
        _renderer->shutdown();
        _renderer.reset();
    }

    if (_platform)
    {
        _platform->shutdown();
        _platform.reset();
    }

    XIHE_CORE_INFO("羲和引擎正在退出");
    Context::Destroy();
    return true;
}
