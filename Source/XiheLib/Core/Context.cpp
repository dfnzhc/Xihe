/**
 * @File Context.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Context.hpp"
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
    // XIHE_ASSERT(s_Instance == nullptr, "EngineContext already exists!");
    sInstance = new Context();

    return sInstance != nullptr;
}

void Context::Destroy()
{
    if (sInstance) {
        delete sInstance;
        sInstance = nullptr;
    }
}

Context& Context::Get()
{
    // XIHE_ASSERT(s_Instance != nullptr, "EngineContext has not been created!");
    return *sInstance;
}
