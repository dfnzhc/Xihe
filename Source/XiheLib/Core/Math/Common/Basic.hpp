/**
 * @File Basic.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/4
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <cstdlib>
#include <cmath>
#include "Core/Math/Common/Constants.hpp"

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

#undef XIHE_DEFINE_COMMON_FUNC

template<cSignedType T>
constexpr T Abs(T x) noexcept { return x < 0 ? -x : x; }

template<cFloatType T>
XIHE_ALWAYS_INLINE T Log(T x) noexcept
{
    using std::log;
    return log(x);
}

template<cFloatType T>
XIHE_ALWAYS_INLINE T Log2(T x) noexcept
{
    using std::log2;
    return log2(x);
}

template<cFloatType T>
constexpr T ATan2(T y, T x) noexcept { return std::atan2(y, x); }

template<typename T, typename... Ts>
constexpr auto Min(T a, T b, Ts... vals) noexcept
{
    const auto m = a < b ? a : b;
    if constexpr (sizeof...(vals) > 0) { return Min(m, T(vals)...); }

    return m;
}

template<typename T, typename... Ts>
constexpr auto Max(T a, T b, Ts... vals) noexcept
{
    const auto m = a > b ? a : b;
    if constexpr (sizeof...(vals) > 0) { return Max(m, T(vals)...); }

    return m;
}

template<cArithmeticType T>
constexpr bool Equal(T x, T y) noexcept { return x == y; }

template<cArithmeticType T>
constexpr bool NotEqual(T x, T y) noexcept { return not Equal(x, y); }

template<cArithmeticType T>
XIHE_ALWAYS_INLINE T FMA(T a, T b, T c) noexcept
{
    using std::fma;
    return fma(a, b, c);
}

template<cArithmeticType T>
constexpr cFloatType auto Radians(T x) noexcept { return static_cast<f64>(x) * 0.01745329251994329576923690768489; }

template<cArithmeticType T>
constexpr cFloatType auto Degrees(T x) noexcept { return static_cast<f64>(x) * 57.295779513082320876798154814105; }

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
        return v_2 * v_2 * Pow<T, N & 1>(v);
    }
}

template<cArithmeticType T>
constexpr T Pow2(T v) noexcept { return Pow<T, 2>(v); }

template<cArithmeticType T>
XIHE_ALWAYS_INLINE T Clamp(T x, T lo, T hi) noexcept { return Min(Max(x, lo), hi); }

template<cArithmeticType T>
XIHE_ALWAYS_INLINE T ClampHigh(T x, T hi) noexcept { return Min(Max(x, Zero<T>()), hi); }

template<cArithmeticType T>
XIHE_ALWAYS_INLINE T ClampNormal(T x) noexcept { return Min(Max(x, Zero<T>()), One<T>()); }

template<cIntegralType T>
constexpr T MidPoint(T a, T b) noexcept { return ((a ^ b) >> 1) + (a & b); }

template<cFloatType T>
constexpr T MidPoint(T a, T b) noexcept
{
    constexpr T low = std::numeric_limits<T>::min() * 2;
    constexpr T high = std::numeric_limits<T>::max() / 2;

    const T abs_a = Abs(a);
    const T abs_b = Abs(b);

    if (abs_a <= high && abs_b <= high)
        return (a + b) / 2;
    if (abs_a < low)
        return a + b / 2;
    if (abs_b < low)
        return b + a / 2;
    return a / 2 + b / 2;
}
} // namespace xihe