/**
 * @File Error.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "Error.hpp"
#include <stacktrace>

using namespace xihe;

XIHE_NORETURN void xihe::ThrowException(const std::source_location& loc, std::string_view msg)
{
    std::string fullMsg = std::format("{}\n\nAt: {}:{} ({})", msg, loc.file_name(), loc.line(), loc.function_name());
    fullMsg += std::format("\n\nStacktrace:\n{}", std::stacktrace::current(1));

    throw xihe::RuntimeError(fullMsg);
}

XIHE_NORETURN void xihe::ReportAssertion(const std::source_location& loc, std::string_view cond, std::string_view msg)
{
    // clang-format off
    std::string fullMsg = msg.empty()
        ? std::format("Condition failed: '{}'", cond)
        : std::format("Condition failed: '{}'\nMessage: {}", cond, msg);

    fullMsg += std::format("\n\nAt: {}:{} ({})", loc.file_name(), loc.line(), loc.function_name());
    // clang-format on
    fullMsg += std::format("\n\nStacktrace:\n{}", std::stacktrace::current(1));

    throw xihe::AssertionError(fullMsg);
}