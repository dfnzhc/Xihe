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
#include "Core/Services/Logger.hpp"

namespace xihe {

class XIHE_API Application
{
public:
    virtual ~Application() = default;

    bool run()
    {
        if (!Context::Create()) { return false; }
        const Logger* logger = Context::Get().getLogger();
        Log(logger, Logger::Core, Logger::Info, "Application starting...");

        _running = onInit();
        while (_running) {
            _running = onTick();
        }

        onShutdown();
        Log(logger, Logger::Core, Logger::Info, "Application exiting.");
        Context::Destroy();
        return true;
    }

protected:
    virtual bool onInit() { return true; }
    virtual bool onTick() { return false; }
    virtual void onShutdown() {}

    std::atomic<bool> _running{false};
};

} // namespace xihe