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
#include <source_location>

#include "Core/Base/Defines.hpp"

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

// 保证唯一性的作用域日志宏
#define XIHE_CONCAT_INNER(a, b) a##b
#define XIHE_CONCAT(a, b) XIHE_CONCAT_INNER(a, b)
#define XIHE_SCOPED_LOG(name) ::xihe::LogScope XIHE_CONCAT(scoped_, __COUNTER__)
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
inline constexpr void Log(const Logger* logger, Logger::Type type, Logger::Level level, const std::source_location& location, std::format_string<Args...> format, Args&&... args)
{
    if (logger == nullptr)
        return;
    logger->log(type, level, std::format("{} ('{}' {}:{})", std::format(format, std::forward<Args>(args)...), location.function_name, location.file_name(), location.line));
}
// clang-format on
} // namespace xihe