/**
 * @File Utils.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/6
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Common/Basic.hpp"

namespace xihe {
inline Size SplitMix64(u64 state) noexcept
{
    state += 0x9E3779B97f4A7C15;

    auto result = state;
    result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
    result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
    return result ^ (result >> 31);
}
} // namespace xihe