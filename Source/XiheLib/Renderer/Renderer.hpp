/**
 * @File Renderer.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>

#include "Core/Base/Defines.hpp"

namespace xihe {
class Platform;
class Window;

class XIHE_API Renderer
{
public:
    enum class Backend { Auto, Vulkan, D3D12, Metal, Null };

    explicit Renderer(Backend backend = Backend::Null);
    ~Renderer();

    bool initialize(Platform* platform);
    void shutdown();

    void setMainWindow(Window* window);

    void beginFrame(double deltaSeconds);
    void render();   // 触发渲染图的编译/执行
    void endFrame(); // 提交/呈现/帧末清理

    // 运行时切换 backend
    bool switchBackend(Backend newBackend);

private:
};

XIHE_API std::unique_ptr<Renderer> CreateRenderer(Renderer::Backend backend = Renderer::Backend::Auto);
} // namespace xihe
