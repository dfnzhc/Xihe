/**
 * @File PCG.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Random/Internal.hpp"

namespace xihe {
class PCG32Engine
{
public:
    using ResultType = u32;
    static constexpr u64 kMultiplier = 6364136223846793005ULL;

    explicit PCG32Engine(u64 seedVal = 0x853c49e6748fea9bULL) noexcept { seed(seedVal); }

    void seed(u64 seedVal) noexcept
    {
        _state = 0;
        setStream(0xDA3E39CB94B95BDBULL);
        auto _ = operator()();
        _state += seedVal;
        _ = operator()();
    }

    void setStream(u64 streamId) noexcept { _increment = (streamId << 1u) | 1u; }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const u64 oldState = _state;
        _state = oldState * kMultiplier + _increment;
        u32 xorshifted = static_cast<u32>(((oldState >> 18u) ^ oldState) >> 27u);
        u32 rot = static_cast<u32>(oldState >> 59u);
        return RotateRight(xorshifted, static_cast<int>(rot));
    }

    // 计算前进/后退 n 步后的状态（n 可为负）
    void advance(i64 delta) noexcept
    {
        if (delta == 0)
            return;

        auto step = [&](u64 mult, u64 plus, u64 iters) {
            u64 accMult = 1u;
            u64 accPlus = 0u;
            while (iters > 0) {
                if (iters & 1u) {
                    accMult = accMult * mult;
                    accPlus = accPlus * mult + plus;
                }
                plus = (mult + 1u) * plus;
                mult = mult * mult;
                iters >>= 1u;
            }
            _state = accMult * _state + accPlus;
        };

        if (delta > 0) { step(kMultiplier, _increment, static_cast<u64>(delta)); }
        else {
            // 计算乘子在 2^64 模数下的逆元（kMultiplier 为奇数，一定可逆）
            auto inv64 = [](u64 a) {
                u64 x = 1;
                // Newton-Raphson 迭代，6 次即可覆盖 64 位
                for (int i = 0; i < 6; ++i)
                    x *= (2 - a * x);
                return x;
            };

            const u64 invMult = inv64(kMultiplier);
            // 逆过程的一步：prev = invMult * cur + (-invMult)*increment
            const u64 invPlus = static_cast<u64>(0) - invMult * _increment;
            step(invMult, invPlus, static_cast<u64>(-delta));
        }
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept { return 0; }
    XIHE_NODISCARD static constexpr ResultType max() noexcept { return std::numeric_limits<ResultType>::max(); }

private:
    u64 _state{0};
    u64 _increment{0};
};

static_assert(SeedableEngine<PCG32Engine>);
static_assert(StreamableEngine<PCG32Engine>);
static_assert(JumpableEngine<PCG32Engine>);
} // namespace xihe