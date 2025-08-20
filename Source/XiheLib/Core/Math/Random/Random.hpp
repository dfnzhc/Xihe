/**
 * @File Random.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Random/Engines/SplitMix.hpp"
#include "Core/Math/Random/Engines/PCG.hpp"
#include "Core/Math/Random/Engines/Xoshiro.hpp"
#include "Core/Math/Random/Engines/Romu.hpp"
#include "Core/Math/Random/Uniform.hpp"
#include "Core/Base/Error.hpp"

namespace xihe {
template <SeedableEngine E>
class RandomEngine
{
public:
    using engine_type = E;
    using ResultType  = typename E::ResultType;

    constexpr explicit RandomEngine(uint64_t seed_val) :
        _engine(seed_val)
    {
    }

    RandomEngine() :
        _engine(GetEntropy())
    {
    }

    constexpr void seed(uint64_t seed_val)
    {
        _engine.seed(seed_val);
    }

    XIHE_NODISCARD constexpr ResultType operator()()
    {
        return _engine();
    }

    constexpr void setStream(uint64_t stream_id) requires StreamableEngine<E>
    {
        _engine.setStream(stream_id);
    }

    constexpr void advance(int64_t delta) requires JumpableEngine<E>
    {
        _engine.advance(delta);
    }

    XIHE_NODISCARD static constexpr ResultType min()
    {
        return E::min();
    }

    XIHE_NODISCARD static constexpr ResultType max()
    {
        return E::max();
    }

private:
    engine_type _engine;
};

using SplitMix64Random   = RandomEngine<SplitMix64Engine>;
using SplitMix32Random   = RandomEngine<SplitMix32Engine>;
using PCG32Random        = RandomEngine<PCG32Engine>;
using Xoshiro256PPRandom = RandomEngine<Xoshiro256PPEngine>;
using Xoshiro128PPRandom = RandomEngine<Xoshiro128PPEngine>;
using RomuTrio32Random   = RandomEngine<RomuTrio32Engine>;
using RomuMono32Random   = RandomEngine<RomuMono32Engine>;
using RomuDuoJr64Random  = RandomEngine<RomuDuoJr64Engine>;

template <SeedableEngine E>
RandomEngine<E>& GetThreadLocalRandomEngine()
{
    thread_local RandomEngine<E> rng(GetEntropy());
    return rng;
}

using DefaultPRNG = PCG32Engine;

XIHE_ALWAYS_INLINE RandomEngine<DefaultPRNG>& ThreadLocalRng()
{
    return GetThreadLocalRandomEngine<DefaultPRNG>();
}

template <cIntegralType T>
XIHE_NODISCARD XIHE_ALWAYS_INLINE T Uniform(T minVal, T maxVal)
{
    auto& rng = ThreadLocalRng();
    return UniformInt<T>(rng, minVal, maxVal);
}

template <cFloatType T>
XIHE_NODISCARD XIHE_ALWAYS_INLINE T Uniform(T minVal, T maxVal)
{
    auto& rng = ThreadLocalRng();
    const T u = UniformReal01<T>(rng);
    return minVal + (maxVal - minVal) * u;
}

template <cFloatType T>
XIHE_NODISCARD XIHE_ALWAYS_INLINE T uniform01()
{
    auto& rng = ThreadLocalRng();
    return UniformReal01<T>(rng);
}

template <typename T>
XIHE_NODISCARD XIHE_ALWAYS_INLINE const T& Choose(std::initializer_list<T> list)
{
    XIHE_CHECK(list.size() > 0, "Choose: empty initializer_list");
    const auto idx = Uniform<Size>(0, list.size() - 1);
    return *(list.begin() + idx);
}

template <typename Container>
XIHE_ALWAYS_INLINE void Shuffle(Container& container)
{
    const Size n = container.size();
    if (n <= 1)
        return;
    for (Size i = n - 1; i > 0; --i)
    {
        const Size j = Uniform<Size>(0, i);
        using std::swap;
        swap(container[i], container[j]);
    }
}
} // namespace xihe
