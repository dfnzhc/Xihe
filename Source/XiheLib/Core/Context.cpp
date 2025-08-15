/**
 * @File Context.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Context.hpp"

#include "Base/Error.hpp"
#include "Services/Logger.hpp"
#include "Events/EventBus.hpp"

using namespace xihe;

Context::Context()
{
    _logger = std::make_unique<Logger>();
    _logger->startup();
    _events = std::make_unique<EventBus>();
}

Context::~Context()
{
    if (_logger != nullptr) {
        _logger->shutdown();
        _logger.reset();
        _logger = nullptr;
    }
    if (_events != nullptr) {
        _events.reset();
    }
}

const Logger* Context::getLogger() const { return _logger.get(); }

EventBus& Context::events() { return *_events; }

bool Context::Create()
{
    std::scoped_lock lock(sMutex);
    if (sInstance != nullptr) { return true; } // 幂等
    sInstance = new Context();
    return sInstance != nullptr;
}

void Context::Destroy()
{
    std::scoped_lock lock(sMutex);
    if (sInstance == nullptr) { return; }
    delete sInstance;
    sInstance = nullptr;
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
