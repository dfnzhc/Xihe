/**
 * @File Internal.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/6
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <thread>
#include "Core/Base/Defines.hpp"

namespace xihe {
inline Size HardwareConcurrency() noexcept
{
    const auto detectedCount     = std::thread::hardware_concurrency();
    constexpr auto fallbackCount = 8;
    return detectedCount ? detectedCount : fallbackCount;
}
} // namespace xihe
