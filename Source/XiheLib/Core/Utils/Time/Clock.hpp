/**
 * @File Clock.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Defines.hpp"
#include <chrono>

namespace xihe {
using Clock = std::chrono::steady_clock;
using Duration = Clock::duration;
using TimePoint = Clock::time_point;

using NanoSeconds = std::chrono::duration<f64, std::chrono::nanoseconds::period>;
using MicroSeconds = std::chrono::duration<f64, std::chrono::microseconds::period>;
using MilliSeconds = std::chrono::duration<f64, std::chrono::milliseconds::period>;
using Seconds = std::chrono::duration<f64, std::chrono::seconds::period>;
} // namespace xihe