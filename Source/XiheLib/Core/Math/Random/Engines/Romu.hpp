/**
 * @File Romu.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Random/Internal.hpp"
#include "Core/Math/Random/Engines/SplitMix.hpp"

namespace xihe {
class RomuTrio32Engine
{
public:
    using ResultType = u32;

    constexpr explicit RomuTrio32Engine(u32 seedVal = 0x9E3779B9u) noexcept
    {
        seed(seedVal);
    }

    void seed(u64 seedVal) noexcept
    {
        SplitMix32Engine seeder(As<u32>(seedVal));
        _x = seeder();
        _y = seeder();
        _z = seeder();
    }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const ResultType xp = _x, yp = _y, zp = _z;
        _x                  = 3323815723u * zp;
        _y                  = RotateLeft(yp - xp, 6);
        _z                  = RotateLeft(zp - yp, 22);
        return xp;
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept
    {
        return 0;
    }

    XIHE_NODISCARD static constexpr ResultType max() noexcept
    {
        return std::numeric_limits<ResultType>::max();
    }

private:
    ResultType _x{}, _y{}, _z{};
};

class RomuMono32Engine
{
public:
    using ResultType = u32;

    constexpr explicit RomuMono32Engine(u32 seedVal = 0xCAFEBABEu) noexcept
    {
        seed(seedVal);
    }

    void seed(u64 seedVal) noexcept
    {
        SplitMix32Engine seeder(As<u32>(seedVal));
        _s = seeder();
        for (int i = 0; i < 10; ++i)
        {
            auto _ = operator()();
        }
    }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const ResultType result = _s >> 16;
        _s *= 3611795771u;
        _s = RotateLeft(_s, 12);
        return result;
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept
    {
        return 0;
    }

    XIHE_NODISCARD static constexpr ResultType max() noexcept
    {
        return std::numeric_limits<ResultType>::max();
    }

private:
    u32 _s{};
};

class RomuDuoJr64Engine
{
public:
    using ResultType = u64;

    constexpr explicit RomuDuoJr64Engine(u64 seedVal = 0xDEADBEEFCAFED00DULL) noexcept
    {
        seed(seedVal);
    }

    void seed(u64 seedVal) noexcept
    {
        SplitMix64Engine seeder(seedVal);
        _x = seeder();
        _y = seeder();
    }

    XIHE_NODISCARD constexpr ResultType operator()() noexcept
    {
        const ResultType res = _x;
        _x                   = 15241094284759029579ULL * _y;
        _y                   = RotateLeft(_y - res, 27);
        return res;
    }

    XIHE_NODISCARD static constexpr ResultType min() noexcept
    {
        return 0;
    }

    XIHE_NODISCARD static constexpr ResultType max() noexcept
    {
        return std::numeric_limits<ResultType>::max();
    }

private:
    ResultType _x{}, _y{};
};

static_assert(SeedableEngine<RomuTrio32Engine>);
static_assert(SeedableEngine<RomuMono32Engine>);
static_assert(SeedableEngine<RomuDuoJr64Engine>);
} // namespace xihe
