/**
 * @File Concepts.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <concepts>
#include "Core/Base/Defines.hpp"

namespace xihe {

// clang-format off
template<typename T> concept cBoolType = std::is_same_v<bool, T>;
template<typename T> concept cU32Type  = std::is_same_v<u32, T>;
template<typename T> concept cU64Type  = std::is_same_v<u64, T>;
template<typename T> concept cF32Type  = std::is_same_v<f32, T>;
template<typename T> concept cF64Type  = std::is_same_v<f64, T>;

template<typename T> concept cSignedType     = std::is_signed_v<T>;
template<typename T> concept cUnsignedType   = std::is_unsigned_v<T>;
template<typename T> concept cIntegralType   = std::is_integral_v<T>;
template<typename T> concept cFloatType      = std::is_floating_point_v<T>;
template<typename T> concept cArithmeticType = std::is_arithmetic_v<T>;
// clang-format on

template<typename T, typename U>
concept cConvertible = (not std::is_same_v<T, U>) and std::is_convertible_v<T, U> and std::is_convertible_v<U, T>;

template<class>
inline constexpr bool kAlwaysFalse = false;

// 用于数值类型转换
template<cArithmeticType T>
constexpr T As(cArithmeticType auto f) noexcept
    requires std::is_nothrow_convertible_v<decltype(f), T> { return static_cast<T>(f); }
} // namespace xihe