/**
 * @File Uniform.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <type_traits>
#include <limits>

#include "Core/Math/Random/Internal.hpp"

namespace xihe {
// 均匀生成 [min, max] 的整数（包含上界）
template <cIntegralType T, BasicEngine E>
XIHE_NODISCARD constexpr T UniformInt(E& gen, T minVal, T maxVal) noexcept
{
    using UnsignedT = std::make_unsigned_t<T>;
    using EngineT   = typename E::ResultType;
    using CommonT   = std::common_type_t<UnsignedT, EngineT>;

    const CommonT range = CommonT(maxVal) - CommonT(minVal);

    // PRNG 参数
    constexpr CommonT prngMin   = CommonT(E::min());
    constexpr CommonT prngMax   = CommonT(E::max());
    constexpr CommonT prngRange = prngMax - prngMin;
    constexpr CommonT typeRange = std::numeric_limits<CommonT>::max();

    // 快速路径：位均匀 PRNG（min==0 且 max==类型最大值）→ Lemire 算法
    if constexpr (prngMin == 0 && prngMax == typeRange)
    {
        using WideT = std::conditional_t<(sizeof(CommonT) > 4 || sizeof(UnsignedT) > 4), u128, u64>;

        CommonT ext   = range + CommonT(1); // [0, ext)
        WideT product = WideT(CommonT(gen())) * ext;
        CommonT low   = CommonT(product);
        if (low < ext)
        {
            const CommonT threshold = -ext % ext;
            while (low < threshold)
            {
                product = WideT(CommonT(gen())) * ext;
                low     = CommonT(product);
            }
        }
        const int digitsU = std::numeric_limits<UnsignedT>::digits;
        return static_cast<T>(minVal + CommonT(product >> digitsU));
    }
    else
    {
        // 通用路径（两次除法法）
        const CommonT ext     = range + CommonT(1);
        const CommonT scaling = prngRange / ext;
        const CommonT past    = ext * scaling;

        CommonT x{};
        do
        {
            x = CommonT(gen()) - prngMin;
        }
        while (x >= past);
        const CommonT val = x / scaling;
        return static_cast<T>(CommonT(minVal) + val);
    }
}

// 均匀生成 [0, 1) 的浮点数（f32、f64）
template <cFloatType T, BasicEngine E>
XIHE_NODISCARD constexpr T UniformReal01(E& gen) noexcept
{
    return GenerateCanonical<T>(gen);
}
} // namespace xihe
