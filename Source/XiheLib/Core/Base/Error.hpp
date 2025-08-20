/**
 * @File Error.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

#include "Core/Base/Defines.hpp"

namespace xihe {
// clang-format off
class Exception : public std::exception
{
public:
    Exception() noexcept = default;
    Exception(const Exception& other) noexcept : exception(other) { _pWhat = other._pWhat; }
    explicit Exception(std::string_view what) : _pWhat(std::string(what)) { }
    ~Exception() override = default;

    Exception& operator=(const Exception&) = delete;
    XIHE_NODISCARD const char* what() const noexcept override { return _pWhat.c_str(); }

protected:
    std::string _pWhat;
};

class RuntimeError : public Exception
{
public:
    RuntimeError() noexcept = default;
    RuntimeError(const RuntimeError& other) noexcept : Exception(other) { _pWhat = other._pWhat; }
    explicit RuntimeError(std::string_view what) : Exception(what) { }

    ~RuntimeError() override = default;
};

class AssertionError : public Exception
{
public:
    AssertionError() noexcept = default;
    AssertionError(const AssertionError& other) noexcept : Exception(other) { _pWhat = other._pWhat; }
    explicit AssertionError(std::string_view what) : Exception(what) { }

    ~AssertionError() override = default;
};

// clang-format on

XIHE_NORETURN XIHE_API void ThrowException(const std::source_location& loc, std::string_view msg);
XIHE_NORETURN XIHE_API void ReportAssertion(const std::source_location& loc, std::string_view cond,
                                            std::string_view msg = {});

namespace details {
    XIHE_NORETURN inline void ThrowException(const std::source_location& loc, std::string_view msg)
    {
        ::xihe::ThrowException(loc, msg);
    }

    template <typename... Args>
    XIHE_NORETURN inline void ThrowException(const std::source_location& loc, std::format_string<Args...> fmt,
                                             Args&&... args)
    {
        ::xihe::ThrowException(loc, std::format(fmt, std::forward<Args>(args)...));
    }

    XIHE_NORETURN inline void ReportAssertion(const std::source_location& loc, std::string_view cond)
    {
        ::xihe::ReportAssertion(loc, cond);
    }

    XIHE_NORETURN inline void ReportAssertion(const std::source_location& loc, std::string_view cond, std::string_view msg)
    {
        ::xihe::ReportAssertion(loc, cond, msg);
    }

    template <typename... Args>
    XIHE_NORETURN inline void ReportAssertion(const std::source_location& loc, std::string_view cond,
                                              std::format_string<Args...> fmt, Args&&... args)
    {
        ::xihe::ReportAssertion(loc, cond, std::format(fmt, std::forward<Args>(args)...));
    }
} // namespace details

#define XIHE_THROW(...) ::xihe::details::ThrowException(std::source_location::current(), __VA_ARGS__)

#define XIHE_CHECK(cond, ...)        \
    do {                             \
        if (!(cond)) {               \
            XIHE_DEBUG_BREAK();      \
            XIHE_THROW(__VA_ARGS__); \
        }                            \
    } while (0)

#define XIHE_UNIMPLEMENTED() XIHE_THROW("Not Implemented!!")

#define XIHE_UNREACHABLE() XIHE_THROW("You shall not PASS!!!")

#define XIHE_ASSERT(cond, ...)                                                                                   \
    do {                                                                                                         \
        if (!(cond)) {                                                                                           \
            ::xihe::details::ReportAssertion(std::source_location::current(), #cond __VA_OPT__(, ) __VA_ARGS__); \
        }                                                                                                        \
    } while (0)

#define XIHE_ASSERT_OP(a, b, OP)                                                                                                           \
    do {                                                                                                                                   \
        if (!((a)OP(b))) {                                                                                                                 \
            ::xihe::details::ReportAssertion(std::source_location::current(), std::format("{} {} {} ({} {} {})", #a, #OP, #b, a, #OP, b)); \
        }                                                                                                                                  \
    } while (0)

#define XIHE_ASSERT_EQ(a, b) XIHE_ASSERT_OP(a, b, ==)
#define XIHE_ASSERT_NE(a, b) XIHE_ASSERT_OP(a, b, !=)
#define XIHE_ASSERT_GE(a, b) XIHE_ASSERT_OP(a, b, >=)
#define XIHE_ASSERT_GT(a, b) XIHE_ASSERT_OP(a, b, >)
#define XIHE_ASSERT_LE(a, b) XIHE_ASSERT_OP(a, b, <=)
#define XIHE_ASSERT_LT(a, b) XIHE_ASSERT_OP(a, b, <)

#ifdef XIHE_DEBUG

#  define XIHE_DEBUG_ASSERT(cond, ...)   XIHE_ASSERT(cond, __VA_ARGS__)
#  define XIHE_DEBUG_ASSERT_OP(a, b, OP) XIHE_ASSERT_OP(a, b, OP)

#  define XIHE_DEBUG_ASSERT_EQ(a, b) XIHE_DEBUG_ASSERT_OP(a, b, ==)
#  define XIHE_DEBUG_ASSERT_NE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, !=)
#  define XIHE_DEBUG_ASSERT_GE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, >=)
#  define XIHE_DEBUG_ASSERT_GT(a, b) XIHE_DEBUG_ASSERT_OP(a, b, >)
#  define XIHE_DEBUG_ASSERT_LE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, <=)
#  define XIHE_DEBUG_ASSERT_LT(a, b) XIHE_DEBUG_ASSERT_OP(a, b, <)

#else // XIHE_DEBUG

#  define XIHE_DEBUG_ASSERT(cond, ...) \
      do {                             \
          {                            \
          }                            \
      } while (0)
#  define XIHE_DEBUG_ASSERT_OP(a, b, OP) \
      do {                               \
          {                              \
          }                              \
      } while (0)
#  define XIHE_DEBUG_ASSERT_EQ(a, b) XIHE_DEBUG_ASSERT_OP(a, b, ==)
#  define XIHE_DEBUG_ASSERT_NE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, !=)
#  define XIHE_DEBUG_ASSERT_GE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, >=)
#  define XIHE_DEBUG_ASSERT_GT(a, b) XIHE_DEBUG_ASSERT_OP(a, b, >)
#  define XIHE_DEBUG_ASSERT_LE(a, b) XIHE_DEBUG_ASSERT_OP(a, b, <=)
#  define XIHE_DEBUG_ASSERT_LT(a, b) XIHE_DEBUG_ASSERT_OP(a, b, <)

#endif // XIHE_DEBUG


// ==================
// Error callback
// ==================

namespace details {
    XIHE_NORETURN inline void CustomTerminateHandler()
    {
        if (auto eptr = std::current_exception())
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch (const std::exception& e)
            {
                // Or use a more robust logging mechanism
                std::cerr << "Terminate called with an exception: " << e.what() << std::endl;
            } catch (...)
            {
                std::cerr << "Terminate called with an unknown exception." << std::endl;
            }
        }
        else
        {
            std::cerr << "Terminate called without an active exception." << std::endl;
        }
        std::abort();
    }

    XIHE_NORETURN inline void SignalHandler(int signal)
    {
        const char* msg;
        if (signal == SIGABRT)
        {
            msg = "Fatal Error: 'SIGABRT' received! Terminating.\n";
        }
        else
        {
            msg = "Fatal Error: Unexpected signal received! Terminating.\n";
        }

        std::cerr << msg;
        std::_Exit(EXIT_FAILURE);
    }
} // namespace details

// ==================
// Guardian callback
// ==================
template <typename F, class... Args>
inline bool Guardian(F callback, Args&&... args)
{
    static_assert(std::is_invocable_v<F, Args...>);
    std::set_terminate(details::CustomTerminateHandler);
    std::signal(SIGABRT, details::SignalHandler);

    bool bResult = false;
    // clang-format off
    try {
        if constexpr (std::is_void_v<std::invoke_result_t<F, Args...>>) {
            std::invoke(callback, std::forward<Args>(args)...);
            bResult = true;
        }
        else {
            bResult = static_cast<bool>(std::invoke(callback, std::forward<Args>(args)...));
        }
    } catch (const AssertionError& e) {
        std::cerr << "Assertion Failed: " << e.what() << std::endl;
    } catch (const std::exception& e) { std::cerr << "Exception: " << e.what() << std::endl; } catch (...) {
        std::cerr << "Unknown Exception" << std::endl;
    }
    // clang-format on

    return bResult;
}
} // namespace xihe
