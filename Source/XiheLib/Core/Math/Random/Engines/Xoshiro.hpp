/**
 * @File Xoshiro.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Random/Internal.hpp"
#include "Core/Math/Random/Engines/SplitMix.hpp"
#include <array>

namespace xihe {
class Xoshiro128PPEngine
{
public:
    using ResultType = u32;

    constexpr explicit Xoshiro128PPEngine(u64 seedVal = 0xCAFEBABEDEADBEEFull) noexcept { seed(seedVal); }

    void seed(u64 seedVal) noexcept
    {
        SplitMix32Engine seeder(As<u32>(seedVal));
        _state[0] = seeder();
        _state[1] = seeder();
        _state[2] = seeder();
        _state[3] = seeder();
    }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const ResultType result = RotateLeft(_state[0] + _state[3], 7) + _state[0];
        const ResultType t = _state[1] << 9;

        _state[2] ^= _state[0];
        _state[3] ^= _state[1];
        _state[1] ^= _state[2];
        _state[0] ^= _state[3];
        _state[2] ^= t;
        _state[3] = RotateLeft(_state[3], 11);
        return result;
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept { return 0; }
    XIHE_NODISCARD static constexpr ResultType max() noexcept { return std::numeric_limits<ResultType>::max(); }

private:
    std::array<u32, 4> _state{};
};

class Xoshiro256PPEngine
{
public:
    using ResultType = u64;

    constexpr explicit Xoshiro256PPEngine(u64 seedVal = 0x1234567890ABCDEFull) noexcept { seed(seedVal); }

    void seed(u64 seedVal) noexcept
    {
        SplitMix64Engine seeder(seedVal);
        _state[0] = seeder();
        _state[1] = seeder();
        _state[2] = seeder();
        _state[3] = seeder();
    }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const ResultType result = RotateLeft(_state[0] + _state[3], 23) + _state[0];
        const ResultType t = _state[1] << 17;

        _state[2] ^= _state[0];
        _state[3] ^= _state[1];
        _state[1] ^= _state[2];
        _state[0] ^= _state[3];
        _state[2] ^= t;
        _state[3] = RotateLeft(_state[3], 45);
        return result;
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept { return 0; }
    XIHE_NODISCARD static constexpr ResultType max() noexcept { return std::numeric_limits<ResultType>::max(); }

private:
    std::array<u64, 4> _state{};
};

static_assert(SeedableEngine<Xoshiro128PPEngine>);
static_assert(SeedableEngine<Xoshiro256PPEngine>);
} // namespace xihe