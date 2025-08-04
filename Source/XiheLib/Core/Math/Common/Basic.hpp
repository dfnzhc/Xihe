/**
 * @File Basic.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/4
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <cstdlib>
#include <cmath>
#include <Core/Math/Common/Constants.hpp>

namespace xihe {

#define XIHE_DEFINE_COMMON_FUNC(Name, Func) \
    template<cFloatType T>                  \
    constexpr T Name(T x) noexcept          \
    {                                       \
        using std::Func;                    \
        return Func(x);                     \
    }

XIHE_DEFINE_COMMON_FUNC(Floor, floor)
XIHE_DEFINE_COMMON_FUNC(Ceil, ceil)
XIHE_DEFINE_COMMON_FUNC(Trunc, trunc)
XIHE_DEFINE_COMMON_FUNC(Round, round)
XIHE_DEFINE_COMMON_FUNC(Sqrt, sqrt)
XIHE_DEFINE_COMMON_FUNC(Exp, exp)
XIHE_DEFINE_COMMON_FUNC(Exp2, exp2)
XIHE_DEFINE_COMMON_FUNC(Log, log)
XIHE_DEFINE_COMMON_FUNC(Log2, log2)
XIHE_DEFINE_COMMON_FUNC(Sin, sin)
XIHE_DEFINE_COMMON_FUNC(Cos, cos)
XIHE_DEFINE_COMMON_FUNC(Tan, tan)
XIHE_DEFINE_COMMON_FUNC(Sinh, sinh)
XIHE_DEFINE_COMMON_FUNC(Cosh, cosh)
XIHE_DEFINE_COMMON_FUNC(Tanh, tanh)
XIHE_DEFINE_COMMON_FUNC(ASin, asin)
XIHE_DEFINE_COMMON_FUNC(ACos, acos)
XIHE_DEFINE_COMMON_FUNC(ATan, atan)
XIHE_DEFINE_COMMON_FUNC(ASinh, asinh)
XIHE_DEFINE_COMMON_FUNC(ACosh, acosh)
XIHE_DEFINE_COMMON_FUNC(ATanh, atanh)
XIHE_DEFINE_COMMON_FUNC(Log10, log10)
XIHE_DEFINE_COMMON_FUNC(Abs, abs)

#undef XIHE_DEFINE_COMMON_FUNC

template<cFloatType T>
constexpr T ATan2(T y, T x) noexcept
{
    return std::atan2(y, x);
}

template<typename T, typename... Ts>
constexpr auto Min(T a, T b, Ts... vals)
{
    const auto m = a < b ? a : b;
    if constexpr (sizeof...(vals) > 0) {
        return Min(m, T(vals)...);
    }

    return m;
}

template<typename T, typename... Ts>
constexpr auto Max(T a, T b, Ts... vals)
{
    const auto m = a > b ? a : b;
    if constexpr (sizeof...(vals) > 0) {
        return Max(m, T(vals)...);
    }

    return m;
}

template<cArithmeticType T>
constexpr bool Equal(T x, T y) noexcept
{
    return x == y;
}

template<cArithmeticType T>
constexpr bool NotEqual(T x, T y) noexcept
{
    return not Equal(x, y);
}

template<cFloatType T>
constexpr T FMA(T a, T b, T c)
{
    using std::fma;
    return fma(a, b, c);
}

constexpr u32 RoundUp(u32 x, u32 y)
{
    if (x == 0)
        return y;
    return ((x + y - 1) / y) * y;
}

constexpr Size AlignUp(Size value, Size alignment)
{
    // Assumes alignment is a power of two
    return (value + alignment - 1) & ~(alignment - 1);
}

template<cArithmeticType T>
constexpr cFloatType auto Radians(T x) noexcept
{
    return static_cast<f64>(x) * 0.01745329251994329576923690768489;
}

template<cArithmeticType T>
constexpr cFloatType auto Degrees(T x) noexcept
{
    return static_cast<f64>(x) * 57.295779513082320876798154814105;
}

template<cArithmeticType T, i32 N>
constexpr T Pow(T v) noexcept
{
    if constexpr (N == 0)
        return 1;
    else if constexpr (N == 1)
        return v;
    else if constexpr (N < 0)
        return 1 / Pow<T, -N>(v);
    else {
        auto v_2 = Pow<T, N / 2>(v);
        return v_2 * v_2 * Pow < T, N & 1 > (v);
    }
}

template<cArithmeticType T>
constexpr T Pow2(T v) noexcept
{
    return Pow<T, 2>(v);
}

} // namespace xihe