/**
 * @File SplitMix.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Random/Internal.hpp"

namespace xihe {
class SplitMix32Engine
{
public:
    using ResultType = u32;

    constexpr explicit SplitMix32Engine(u64 seedVal = 0x9E3779B97f4A7C15ULL) noexcept { seed(seedVal); }

    constexpr void seed(u64 seedVal) noexcept { _state = As<u32>(SplitMix64(seedVal)); }

    constexpr ResultType operator()() noexcept
    {
        _state += 0x9e3779b9u;
        u32 result = _state;
        result = (result ^ (result >> 16)) * 0x21f0aaadu;
        result = (result ^ (result >> 15)) * 0x735a2d97u;
        return result ^ (result >> 15);
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept { return 0; }
    XIHE_NODISCARD static constexpr ResultType max() noexcept { return std::numeric_limits<ResultType>::max(); }

private:
    u32 _state{};
};

class SplitMix64Engine
{
public:
    using ResultType = u64;

    constexpr explicit SplitMix64Engine(u64 seedVal = 0xDEADBEEFCAFED00DULL) noexcept { seed(seedVal); }

    constexpr void seed(u64 seedVal) noexcept { _state = seedVal; }

    constexpr ResultType operator()() noexcept
    {
        u64 z = (_state += 0x9E3779B97f4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept { return 0; }
    XIHE_NODISCARD static constexpr ResultType max() noexcept { return std::numeric_limits<ResultType>::max(); }

private:
    u64 _state{};
};

static_assert(SeedableEngine<SplitMix32Engine>);
static_assert(SeedableEngine<SplitMix64Engine>);
} // namespace xihe