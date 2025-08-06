/**
 * @File Sleep.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/6
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Threading/Common.hpp"
#include "Core/Utils/Time/Clock.hpp"

namespace xihe {
class XIHE_API Sleep
{
    template<class Rep, class Period>
    XIHE_ALWAYS_INLINE static void Spinlock(Duration<Rep, Period> duration)
    {
        const auto StartTimepoint = Clock::now();

        // 防止编译器优化
        volatile std::uint64_t i = 0;
        while (Clock::now() - StartTimepoint < duration)
            ++i;
    }

    template<class Rep, class Period>
    XIHE_ALWAYS_INLINE static void Thread(Duration<Rep, Period> duration) { std::this_thread::sleep_for(duration); }

    template<class Rep, class Period>
    XIHE_ALWAYS_INLINE static void Hybrid(Duration<Rep, Period> duration)
    {
        using ms = std::chrono::duration<f64, std::milli>;

        constexpr auto shortDuration = ms(1);

        constexpr f64 stddevAboveMean = 1;

        thread_local auto errEstimate = ms(5e-3); // estimates should be per-thread,
        thread_local auto errMean = errEstimate; // having these 'static' would introduce
        thread_local auto errM2 = ms(0); // a race condition in a concurrent scenario
        thread_local std::uint64_t count = 1;

        ms remainingDuration = duration;

        while (remainingDuration > errEstimate) {
            const auto start = Clock::now();
            Thread<Rep, Period>(shortDuration);
            const auto end = Clock::now();

            const ms observed = end - start;
            remainingDuration -= observed;

            ++count;

            // Update error estimate with Welford's algorithm
            const ms delta = observed - errMean;
            errMean += delta / count;
            errM2 += delta.count() * (observed - errMean); // intermediate values 'm2' reduce numerical instability
            const auto variance = ms(std::sqrt(errM2.count() / (count - 1)));

            errEstimate = errMean + stddevAboveMean * variance;
        }

        Spinlock(remainingDuration);
    }
};
} // namespace xihe