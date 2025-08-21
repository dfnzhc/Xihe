/**
 * @File Application.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <atomic>

#include "Core/Base/Defines.hpp"
#include "Core/Context.hpp"
#include "Core/Utils/Logger.hpp"
#include "Core/Platform/Platform.hpp"
#include "Renderer/Renderer.hpp"

namespace xihe {
class XIHE_API Application
{
public:
    virtual ~Application() = default;

    bool run();

    void stop()
    {
        _running.store(false, std::memory_order_relaxed);
    }

    XIHE_NODISCARD IPlatform* platform() const
    {
        return _platform.get();
    }

protected:
    virtual bool onInit()
    {
        return true;
    }

    virtual void onTick()
    {
    }

    virtual void onShutdown()
    {
    }

    std::atomic<bool> _running{false};
    std::unique_ptr<IPlatform> _platform;
    std::unique_ptr<Renderer> _renderer;
};
} // namespace xihe
