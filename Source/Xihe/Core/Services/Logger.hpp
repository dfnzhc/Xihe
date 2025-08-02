/**
 * @File Logger.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <format>

#include "Core/Base/Defines.hpp"
#include "Core/Utils/SourceLocation.hpp"

namespace xihe {
class XIHE_API LogIndenter
{
public:
    static void Increase();
    static void Decrease();

    static const std::string& GetIndentString();
    static u8 GetLevel();

private:
    inline thread_local static u8 sIndentLevel = 0;
    inline static const char* sIndentChar = "|   ";
};

class LogScope
{
public:
    LogScope() noexcept { LogIndenter::Increase(); }
    ~LogScope() noexcept { LogIndenter::Decrease(); }
};

#define XIHE_SCOPED_LOG(name) ::xihe::LogScope scoped_##__LINE__##name
#define XIHE_LOG_SCOPE() XIHE_SCOPED_LOG(__FUNCTION__)

// -----------------------------

class XIHE_API Logger
{
public:
    void startup();
    void shutdown();

    enum class Level
    {
        Trace,
        Info,
        Warn,
        Error,
        Fatal
    };

    using enum Level;

    enum class Type
    {
        Core,
        Client
    };

    using enum Type;

    void log(Type type, Level level, std::string_view msg) const;
};

inline constexpr void Log(const Logger* logger, Logger::Type type, Logger::Level level, std::string_view msg)
{
    if (logger == nullptr)
        return;
    logger->log(type, level, msg);
}

template<typename... Args>
inline constexpr void Log(const Logger* logger, Logger::Type type, Logger::Level level, std::format_string<Args...> format, Args&&... args)
{
    if (logger == nullptr)
        return;
    logger->log(type, level, std::format(format, std::forward<Args>(args)...));
}

// clang-format off
template<typename... Args>
inline constexpr void Log(const Logger* logger, Logger::Type type, Logger::Level level, const SourceLocation& location, std::format_string<Args...> format, Args&&... args)
{
    if (logger == nullptr)
        return;
    logger->log(type, level, std::format("{} ('{}' {}:{})", std::format(format, std::forward<Args>(args)...), location.function, location.file, location.line));
}
// clang-format on
} // namespace xihe