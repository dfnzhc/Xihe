/**
 * @File Error.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <exception>
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

} // namespace xihe