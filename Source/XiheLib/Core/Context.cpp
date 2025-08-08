/**
 * @File Context.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Context.hpp"

#include "Base/Error.hpp"
#include "Services/Logger.hpp"

using namespace xihe;

Context::Context()
{
    _logger = std::make_unique<Logger>();
    _logger->startup();
}

Context::~Context()
{
    if (_logger != nullptr) {
        _logger->shutdown();
        _logger.reset();
        _logger = nullptr;
    }
}

const Logger* Context::getLogger() const { return _logger.get(); }

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
