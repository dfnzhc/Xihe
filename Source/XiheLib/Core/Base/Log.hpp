/**
 * @File Log.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Services/Logger.hpp"
#include "Core/Context.hpp"

namespace xihe {
#ifdef XIHE_DEBUG
#define XIHE_CORE_TRACE(...) Log((Context::Get().getLogger()), Logger::Type::Core, Logger::Level::Trace, __VA_ARGS__)
#define XIHE_TRACE(...) Log((Context::Get().getLogger()), Logger::Type::Client, Logger::Level::Trace, __VA_ARGS__)
#else
#define XIHE_CORE_TRACE(...) (void)0
#define XIHE_TRACE(...) (void)0
#endif

// clang-format off
#define XIHE_CORE_INFO(...) Log((Context::Get().getLogger()), Logger::Type::Core, Logger::Level::Info, __VA_ARGS__)
#define XIHE_CORE_WARN(...) Log((Context::Get().getLogger()), Logger::Type::Core, Logger::Level::Warn, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)
#define XIHE_CORE_ERROR(...) Log((Context::Get().getLogger()), Logger::Type::Core, Logger::Level::Error, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)
#define XIHE_CORE_FATAL(...) Log((Context::Get().getLogger()), Logger::Type::Core, Logger::Level::Fatal, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)

#define XIHE_INFO(...) Log((Context::Get().getLogger()), Logger::Type::Client, Logger::Level::Info, __VA_ARGS__)
#define XIHE_WARN(...) Log((Context::Get().getLogger()), Logger::Type::Client, Logger::Level::Warn, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)
#define XIHE_ERROR(...) Log((Context::Get().getLogger()), Logger::Type::Client, Logger::Level::Error, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)
#define XIHE_FATAL(...) Log((Context::Get().getLogger()), Logger::Type::Client, Logger::Level::Fatal, {__FILE__, __LINE__, __FUNCTION__}, __VA_ARGS__)

// clang-format on

} // namespace xihe