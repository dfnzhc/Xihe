/**
 * @File Approximate.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/4
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <Core/Math/Common/Basic.hpp>
#include <Core/Base/Error.hpp>

namespace xihe {

template<cFloatType T>
constexpr bool Approx(T x, T y, f32 eps = kEpsilonF) noexcept
{
    return Abs(x - y) < eps;
}

template<cFloatType T>
constexpr bool NotApprox(T x, T y, f32 eps = kEpsilonF) noexcept
{
    return not Approx(x, y, eps);
}

constexpr f32 ApproxSqrt(f32 x0)
{
    XIHE_DEBUG_ASSERT(x0 >= 0);

    union
    {
        i32 ix;
        f32 x;
    } u{};

    u.x  = x0;
    u.ix = 0x1fbb3f80 + (u.ix >> 1);
    u.x  = 0.5f * (u.x + x0 / u.x);
    u.x  = 0.5f * (u.x + x0 / u.x);
    return u.x;
}

constexpr f32 ApproxCbrt(f32 x0)
{
    XIHE_DEBUG_ASSERT(x0 >= 0);

    union
    {
        i32 ix;
        f32 x;
    } u{};

    u.x  = x0;
    u.ix = u.ix / 4 + u.ix / 16;
    u.ix = u.ix + u.ix / 16;
    u.ix = u.ix + u.ix / 256;
    u.ix = 0x2a5137a0 + u.ix;
    u.x  = 0.33333333f * (2.0f * u.x + x0 / (u.x * u.x));
    u.x  = 0.33333333f * (2.0f * u.x + x0 / (u.x * u.x));
    return u.x;
}

template<cFloatType T>
constexpr T InvApproxSqrt(T x0)
{
    XIHE_DEBUG_ASSERT(x0 > 0);

    if constexpr (sizeof(T) == 4) {
        union
        {
            i32 ix;
            f32 x;
        } u{};

        u.x       = x0;
        f32 xHalf = 0.5f * u.x;
        u.ix      = 0x5f37599e - (u.ix >> 1);
        u.x       = u.x * (1.5f - xHalf * u.x * u.x);
        u.x       = u.x * (1.5f - xHalf * u.x * u.x);
        return u.x;
    }
    else {
        union
        {
            i64 ix;
            f64 x;
        } u{};

        u.x  = x0;
        u.ix = 0x5fe6ec85e8000000LL - (u.ix >> 1);
        return u.x;
    }
}

} // namespace xihe