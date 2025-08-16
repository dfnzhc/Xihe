/**
 * @File Context.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <mutex>
#include "Core/Base/Defines.hpp"

namespace xihe {
class Logger;
class EventBus;
class ConfigManager;

// Context 会提供一些通用服务对象，这些对象应该负责自身的多线程安全
class XIHE_API Context
{
public:
    Context();
    ~Context();

    const Logger* getLogger() const;
    EventBus& events();
    ConfigManager& configManager();

    // -----------------------------
    static bool Create();
    static void Destroy();

    static Context& Get();
    static Context* TryGet();

private:
    bool isFinalized() const;

private:
    inline static Context* sInstance = nullptr;

    std::unique_ptr<Logger> _logger;
    std::unique_ptr<EventBus> _events;
    std::unique_ptr<ConfigManager> _configManager;
};
} // namespace xihe
