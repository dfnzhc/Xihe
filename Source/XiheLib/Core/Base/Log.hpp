/**
 * @File Log.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "../Utils/Logger.hpp"
#include "Core/Context.hpp"

namespace xihe {
#ifdef XIHE_DEBUG
#define XIHE_CORE_TRACE(...) do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Core, Logger::Level::Trace, __VA_ARGS__); } while(0)
#define XIHE_TRACE(...)      do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Client, Logger::Level::Trace, __VA_ARGS__); } while(0)
#else
#define XIHE_CORE_TRACE(...) (void)0
#define XIHE_TRACE(...) (void)0
#endif

// clang-format off
#define XIHE_CORE_INFO(...)  do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Core, Logger::Level::Info, __VA_ARGS__); } while(0)
#define XIHE_CORE_WARN(...)  do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Core, Logger::Level::Warn, std::source_location::current(), __VA_ARGS__); } while(0)
#define XIHE_CORE_ERROR(...) do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Core, Logger::Level::Error, std::source_location::current(), __VA_ARGS__); } while(0)
#define XIHE_CORE_FATAL(...) do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Core, Logger::Level::Fatal, std::source_location::current(), __VA_ARGS__); } while(0)

#define XIHE_INFO(...)  do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Client, Logger::Level::Info, __VA_ARGS__); } while(0)
#define XIHE_WARN(...)  do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Client, Logger::Level::Warn, std::source_location::current(), __VA_ARGS__); } while(0)
#define XIHE_ERROR(...) do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Client, Logger::Level::Error, std::source_location::current(), __VA_ARGS__); } while(0)
#define XIHE_FATAL(...) do { auto* ctx = Context::TryGet(); if (ctx) Log((ctx->getLogger()), Logger::Type::Client, Logger::Level::Fatal, std::source_location::current(), __VA_ARGS__); } while(0)

// clang-format on
} // namespace xihe
