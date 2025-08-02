/**
 * @File Logger.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Logger.hpp"

#ifdef XIHE_DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
XIHE_CLANG_DISABLE_WARNING("-Wswitch-enum")
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wduplicate-enum")
XIHE_CLANG_DISABLE_WARNING("-Wmissing-noreturn")
XIHE_CLANG_DISABLE_WARNING("-Wnonportable-system-include-path")
XIHE_CLANG_DISABLE_WARNING("-Wfloat-equal")
XIHE_CLANG_DISABLE_WARNING("-Wimplicit-int-conversion")
XIHE_CLANG_DISABLE_WARNING("-Wlanguage-extension-token")
XIHE_CLANG_DISABLE_WARNING("-Wcovered-switch-default")
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
XIHE_POP_WARNING

using namespace xihe;

namespace {
std::shared_ptr<spdlog::logger> sCoreLogger = nullptr;
std::shared_ptr<spdlog::logger> sClientLogger = nullptr;
} // namespace

void LogIndenter::Increase()
{
    // 最大 10 个层级
    sIndentLevel = static_cast<u8>(std::min(10 - 1, sIndentLevel + 1));
}

void LogIndenter::Decrease()
{
    // 至少 0 级
    sIndentLevel = static_cast<u8>(std::max(0, sIndentLevel - 1));
}

const std::string& LogIndenter::GetIndentString()
{
    static thread_local std::array<std::string, 10> indentCache;

    if (indentCache[sIndentLevel].empty()) {
        std::string& temp = indentCache[sIndentLevel];
        for (int i = 0; i < sIndentLevel; ++i) { temp += sIndentChar; }
    }
    return indentCache[sIndentLevel];
}

u8 LogIndenter::GetLevel() { return sIndentLevel; }

void Logger::startup()
{
#ifdef _WIN32
    // 设置控制台代码页为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif

    spdlog::init_thread_pool(8192, 1);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Xihe.log", true);

    // 控制台格式: [时间] [线程ID] [Logger名]: 消息
    console_sink->set_pattern("%^[%T] [%t] %4n: %v%$");

    // 文件格式: [日期 时间.毫秒] [级别] [Logger名] [线程ID] 消息
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%8l] [%4n] [thread %t] %v");

#ifdef XIHE_DEBUG
    // Debug 模式下，所有级别的日志都输出到所有地方
    console_sink->set_level(spdlog::level::trace);
    file_sink->set_level(spdlog::level::trace);
    spdlog::set_level(spdlog::level::trace);
#else
    // Release 模式下，只显示 Info 及以上级别
    console_sink->set_level(spdlog::level::info);
    file_sink->set_level(spdlog::level::info);
    spdlog::set_level(spdlog::level::info);
#endif

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

    // clang-format off
    sCoreLogger = std::make_shared<spdlog::async_logger>("Xihe", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    sClientLogger = std::make_shared<spdlog::async_logger>("App", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    // clang-format on

#ifdef XIHE_DEBUG
    sCoreLogger->set_level(spdlog::level::trace);
    sClientLogger->set_level(spdlog::level::trace);
#else
    sCoreLogger->set_level(spdlog::level::info);
    sClientLogger->set_level(spdlog::level::info);
#endif

    spdlog::register_logger(sCoreLogger);
    spdlog::register_logger(sClientLogger);
}

void Logger::shutdown() { spdlog::shutdown(); }

void Logger::log(Type type, Level level, std::string_view msg) const
{
    auto logger = sCoreLogger;
    if (type == Client)
        logger = sClientLogger;

    // clang-format off
    switch (level) {
    case Trace   : SPDLOG_LOGGER_TRACE(logger.get(), "{}{}", LogIndenter::GetIndentString(), msg); break;
    case Info    : SPDLOG_LOGGER_INFO(logger.get(), "{}{}", LogIndenter::GetIndentString(), msg); break;
    case Warn    : SPDLOG_LOGGER_WARN(logger.get(), "{}{}", LogIndenter::GetIndentString(), msg); break;
    case Error   : SPDLOG_LOGGER_ERROR(logger.get(), "{}{}", LogIndenter::GetIndentString(), msg); break;
    case Fatal   : SPDLOG_LOGGER_CRITICAL(logger.get(), "{}{}", LogIndenter::GetIndentString(), msg); break;
    }
    // clang-format on
}