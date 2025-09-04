/**
 * @File Renderer.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/14
 * @Brief This file is part of Xihe.
 */

#include "Renderer.hpp"

#include "Core/Utils/Logger.hpp"

namespace xihe {
Renderer::Renderer(Backend backend)
{
}

Renderer::~Renderer() = default;

bool Renderer::initialize(Platform* platform)
{
    return true;
}

void Renderer::shutdown()
{
}

void Renderer::setMainWindow(Window* window)
{
}

void Renderer::beginFrame(double deltaSeconds)
{
}

void Renderer::render()
{
}

void Renderer::endFrame()
{
}

bool Renderer::switchBackend(Backend newBackend)
{
    return true;
}

// 工厂函数
std::unique_ptr<Renderer> CreateRenderer(Renderer::Backend backend)
{
    // TODO: 根据 backend 参数选择具体的后端实现
    // 当前默认使用 Null 后端
    if (backend == Renderer::Backend::Auto)
    {
        backend = Renderer::Backend::Null;
    }
    return std::make_unique<Renderer>(backend);
}
} // namespace xihe
