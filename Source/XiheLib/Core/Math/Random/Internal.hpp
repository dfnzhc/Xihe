/**
 * @File Internal.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/6
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Common/Basic.hpp"
#include "Core/Math/Common/Bits.hpp"
#include <chrono>
#include <thread>

namespace xihe {
// -----------------------------
// 定义一些随机数引擎 Concept

// 最基本的随机数引擎
template <typename E>
concept BasicEngine = requires(E e)
{
    // 必须定义一个名为 ResultType 的无符号整数类型
    typename E::ResultType;
    requires std::is_unsigned_v<typename E::ResultType>;

    // 必须可以像函数一样被调用，并返回 ResultType
    { e() } -> std::same_as<typename E::ResultType>;

    // 必须提供静态的 min() 和 max() 方法，用于兼容C++标准库
    { E::min() } -> std::same_as<typename E::ResultType>;
    { E::max() } -> std::same_as<typename E::ResultType>;

    // min() 和 max() 必须是编译期常量，且 min() < max()
    requires std::bool_constant<(E::min() < E::max())>::value;
};

// 可以设置种子的随机数引擎
template <typename E>
concept SeedableEngine = BasicEngine<E> && requires(E e, u64 seed)
{
    E(seed);
    e.seed(seed);
};

// 流随机数引擎，允许产生几个完全独立、永不重叠的随机数序列
template <typename E>
concept StreamableEngine = SeedableEngine<E> && requires(E e, u64 stream_id)
{
    e.setStream(stream_id);
};

// 可跳转的随机数引擎
template <typename E>
concept JumpableEngine = SeedableEngine<E> && requires(E e, i64 delta)
{
    e.advance(delta);
};

namespace details {
    // 计算一个无符号整数的有效位数
    template <std::unsigned_integral T>
    XIHE_NODISCARD consteval int BitWidth(T value) noexcept
    {
        if (value == 0)
            return -1;
        int width = 0;
        while (value >>= 1)
        {
            ++width;
        }
        return width;
    }

    // 计算一个无符号整数包含多少个有效的随机位
    template <BasicEngine E>
    XIHE_NODISCARD constexpr int GetProduceRandomBits() noexcept
    {
        using T              = typename E::ResultType;
        constexpr T prng_min = E::min();
        constexpr T prng_max = E::max();

        constexpr T prng_range = prng_max - prng_min;
        constexpr T type_range = std::numeric_limits<T>::max();

        if constexpr (prng_range < type_range)
        {
            return BitWidth(prng_range + 1);
        }
        else
        {
            return std::numeric_limits<T>::digits;
        }
    }
} // namespace details


// std::generate_canonical 实现，随机生成 [0, 1) 的浮点数
template <std::floating_point F, BasicEngine E>
XIHE_NODISCARD constexpr F GenerateCanonicalGeneric(E& gen)
{
    constexpr int target_bits        = std::numeric_limits<F>::digits;
    constexpr int prng_bits          = details::GetProduceRandomBits<E>();
    constexpr int invocations_needed = (target_bits + prng_bits - 1) / prng_bits;

    constexpr F prng_min_f   = As<F>(E::min());
    constexpr F prng_range_f = As<F>(E::max() - E::min()) + One<F>();

    F result = Zero<F>();
    F factor = One<F>();

    for (int i = 0; i < invocations_needed; ++i)
    {
        result += (As<F>(gen()) - prng_min_f) * factor;
        factor *= prng_range_f;
    }

    result /= factor;

    // 确保结果严格小于1.0
    if (result >= One<F>())
    {
        result = std::nextafter(One<F>(), Zero<F>());
    }

    return result;
}

// std::generate_canonical 实现，随机生成 [0, 1) 的浮点数，对均匀比特位的随机数引擎存在快速处理
template <std::floating_point F, BasicEngine E>
XIHE_NODISCARD constexpr F GenerateCanonical(E& gen)
{
    using EngineResultT = typename E::ResultType;

    if constexpr (E::min() == 0 && (E::max() == std::numeric_limits<EngineResultT>::max()))
    {
        if constexpr (std::is_same_v<F, f64> && sizeof(EngineResultT) >= 8)
        {
            // -- f64 from 64-bit Uniform PRNG
            return (As<f64>(gen() >> 11)) * 0x1.0p-53;
        }
        else if constexpr (std::is_same_v<F, f64> && sizeof(EngineResultT) == 4)
        {
            // -- f64 from 32-bit Uniform PRNG
            const auto high = As<u32>(gen());
            const auto low  = As<u32>(gen());
            return (As<f64>((high << 21) | (low >> 11))) * 0x1.0p-53;
        }
        else if constexpr (std::is_same_v<F, f32> && sizeof(EngineResultT) >= 4)
        {
            // -- f32 from 32/64-bit Uniform PRNG
            return (As<f32>(As<u32>(gen()) >> 8)) * 0x1.0p-24f;
        }
        else
        {
            return GenerateCanonicalGeneric<F>(gen);
        }
    }
    else
    {
        return GenerateCanonicalGeneric<F>(gen);
    }
}

/**
 * @brief 从多种来源收集熵，生成一个高质量的64位种子。
 * @return 一个随机的64位种子值。
 */
XIHE_ALWAYS_INLINE u64 GetEntropy()
{
    // 熵1: 时间戳
    u64 time_entropy = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // 熵2: 堆地址
    auto* heap_ptr   = new char;
    u64 heap_entropy = reinterpret_cast<uintptr_t>(heap_ptr);
    delete heap_ptr;

    // 熵3: 栈地址
    char stack_var;
    u64 stack_entropy = reinterpret_cast<uintptr_t>(&stack_var);

    // 熵4: 线程ID
    u64 thread_entropy = std::hash<std::thread::id>{}(std::this_thread::get_id());

    // 混合
    return SplitMix64(time_entropy ^ heap_entropy ^ stack_entropy ^ thread_entropy);
}

// -----------------------------
// 简单的无符号 128 位整型，用于随机数生成

#if defined(__SIZEOF_INT128__)
using uint128_t = __uint128_t;
#else
struct uint128_t
{
    u64 low;
    u64 high;
    
    constexpr explicit uint128_t(u64 l = 0, u64 h = 0) noexcept : low(l), high(h) {}

    constexpr uint128_t& operator*=(const uint128_t& other) noexcept
    {
        u64 a = this->low;
        u64 b = other.low;

        u64 a_lo = a & 0xFFFFFFFF;
        u64 a_hi = a >> 32;
        u64 b_lo = b & 0xFFFFFFFF;
        u64 b_hi = b >> 32;

        u64 p00 = a_lo * b_lo;
        u64 p01 = a_lo * b_hi;
        u64 p10 = a_hi * b_lo;
        u64 p11 = a_hi * b_hi;

        u64 mid_sum = (p00 >> 32) + (p01 & 0xFFFFFFFF) + (p10 & 0xFFFFFFFF);
            
        u64 result_high_part = p11 + (p01 >> 32) + (p10 >> 32) + (mid_sum >> 32);
        u64 result_low_part = (mid_sum << 32) | (p00 & 0xFFFFFFFF);

        result_high_part += this->low * other.high;
        result_high_part += this->high * other.low;

        this->low = result_low_part;
        this->high = result_high_part;
            
        return *this;
    }

    constexpr explicit operator u64() const noexcept { return low; }
};
#endif

using u128 = uint128_t;
} // namespace xihe
