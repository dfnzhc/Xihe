/**
 * @File Context.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Context.hpp"

#include "Base/Error.hpp"
#include "Utils/Logger.hpp"
#include "Events/EventBus.hpp"
#include "Memory/Memory.hpp"
#include "Utils/ConfigManager.hpp"

using namespace xihe;

Context::Context()
{
}

Context::~Context()
{
}

EventBus& Context::events()
{
    return *_events;
}

ConfigManager& Context::configManager()
{
    return *_configManager;
}

bool Context::Create()
{
    // Context 应该在程序启动的一开始调用，且整个生命周期应该只调用一次 
    if (sInstance != nullptr)
    {
        return true;
    }
    sInstance = new Context();

    XIHE_ASSERT(sInstance);

    Logger::GetInstance().startup();

    sInstance->_events = std::make_unique<EventBus>();

    sInstance->_configManager = std::make_unique<ConfigManager>();
    sInstance->_configManager->loadFromFile();

    return sInstance->isFinalized();
}

void Context::Destroy()
{
    if (sInstance == nullptr)
    {
        return;
    }

    XIHE_SAFE_RESET_PTR(sInstance->_events);

    // 保存当前配置
    if (sInstance->_configManager != nullptr)
    {
        auto _ = sInstance->_configManager->saveToFile();

        XIHE_SAFE_RESET_PTR(sInstance->_configManager);
    }

    Logger::GetInstance().shutdown();

    XIHE_SAFE_DELETE_PTR(sInstance);
}

Context& Context::Get()
{
    XIHE_CHECK(sInstance != nullptr, "EngineContext has not been created!");
    return *sInstance;
}

Context* Context::TryGet()
{
    return sInstance;
}

bool Context::isFinalized() const
{
    if (sInstance->_events == nullptr)
    {
        return false;
    }
    if (sInstance->_configManager == nullptr || !sInstance->_configManager->isLoaded())
    {
        return false;
    }

    return true;
}
