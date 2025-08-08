/**
 * @File Stopwatch.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <atomic>
#include "Core/Base/Defines.hpp"
#include "Core/Utils/Time/Clock.hpp"

namespace xihe::time {
class Stopwatch
{
public:
    Stopwatch() = default;

    XIHE_ALWAYS_INLINE void start()
    {
        if (_running.load(std::memory_order_relaxed))
            return;
        _start = Clock::now();
        _running.store(true, std::memory_order_release);
    }

    XIHE_ALWAYS_INLINE void stop()
    {
        if (!_running.load(std::memory_order_acquire))
            return;
        const auto now = Clock::now();
        _accumulated += std::chrono::duration_cast<Seconds>(now - _start).count();
        _running.store(false, std::memory_order_release);
    }

    XIHE_ALWAYS_INLINE void reset()
    {
        _accumulated = 0.0;
        _running.store(false, std::memory_order_release);
    }

    XIHE_NODISCARD XIHE_ALWAYS_INLINE f64 elapsedSeconds() const
    {
        if (_running.load(std::memory_order_acquire)) {
            const auto now = Clock::now();
            return _accumulated + std::chrono::duration_cast<Seconds>(now - _start).count();
        }
        return _accumulated;
    }

private:
    std::atomic<bool> _running{false};
    TimePoint _start{};
    f64 _accumulated{0.0};
};
} // namespace xihe::time