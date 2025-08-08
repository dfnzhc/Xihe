/**
 * @File RandomTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/7
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Math/Random.hpp>
#include <vector>
#include <algorithm>

using namespace xihe;

namespace mock_engines {
struct Empty {};

// 只满足 BasicEngine
struct Basic
{
    using ResultType = u32;
    ResultType operator()() { return 0; }
    static constexpr ResultType min() { return 0; }
    static constexpr ResultType max() { return std::numeric_limits<ResultType>::max(); }
};

// 满足 SeedableEngine
struct Seedable : Basic
{
    Seedable(u64) {}
    void seed(u64) {}
};

// 满足 StreamableEngine
struct Streamable : Seedable
{
    using Seedable::Seedable;
    void setStream(u64) {}
};

// 满足 JumpableEngine
struct Jumpable : Seedable
{
    using Seedable::Seedable;
    void advance(int64_t) {}
};

// 一个完全实现的引擎
struct Full : Streamable
{
    using Streamable::Streamable;
    void advance(int64_t) {}
};

// 一个标准的、位均匀的32位引擎
struct Uniform32
{
    using ResultType = u32;
    u32 state = 0;

    ResultType operator()() { return state++; }
    static constexpr ResultType min() { return 0; }
    static constexpr ResultType max() { return std::numeric_limits<ResultType>::max(); }
};

static_assert(BasicEngine<Uniform32>);

// 一个标准的、位均匀的64位引擎
using Uniform64 = SplitMix64Engine; // 复用我们实现的真实引擎
static_assert(BasicEngine<Uniform64>);

// 一个非位均匀的引擎 (min != 0)
struct NonUniform
{
    using ResultType = u32;
    u32 state = 1;

    ResultType operator()() { return state++; }
    static constexpr ResultType min() { return 1; } // min 不为 0
    static constexpr ResultType max() { return std::numeric_limits<ResultType>::max(); }
};

static_assert(BasicEngine<NonUniform>);

// 一个只有16位输出的引擎
struct Uniform16
{
    using ResultType = uint16_t;
    uint16_t state = 0;

    ResultType operator()() { return state++; }
    static constexpr ResultType min() { return 0; }
    static constexpr ResultType max() { return std::numeric_limits<ResultType>::max(); }
};

static_assert(BasicEngine<Uniform16>);
} // namespace mock_engines


static_assert(!BasicEngine<mock_engines::Empty>, "Concept Failed: Empty should not be a BasicEngine.");
static_assert(BasicEngine<mock_engines::Basic>, "Concept Failed: Basic should be a BasicEngine.");
static_assert(SeedableEngine<mock_engines::Seedable>, "Concept Failed: Seedable should be a SeedableEngine.");
static_assert(!SeedableEngine<mock_engines::Basic>, "Concept Failed: Basic should not be a SeedableEngine.");
static_assert(StreamableEngine<mock_engines::Streamable>, "Concept Failed: Streamable should be a StreamableEngine.");
static_assert(!StreamableEngine<mock_engines::Jumpable>, "Concept Failed: Jumpable should not be a StreamableEngine.");
static_assert(JumpableEngine<mock_engines::Jumpable>, "Concept Failed: Jumpable should be a JumpableEngine.");
static_assert(!JumpableEngine<mock_engines::Streamable>, "Concept Failed: Streamable should not be a JumpableEngine.");
static_assert(StreamableEngine<mock_engines::Full> && JumpableEngine<mock_engines::Full>, "Concept Failed: Full engine should satisfy all concepts.");

TEST(GenerateCanonical, FastPathCombinations)
{
    constexpr int N = 1000;

    // --- f64 from 64-bit engine ---
    mock_engines::Uniform64 eng64;
    for (int i = 0; i < N; ++i) {
        f64 val = GenerateCanonical<f64>(eng64);
        ASSERT_GE(val, 0.0);
        ASSERT_LT(val, 1.0);
    }

    // --- f64 from 32-bit engine ---
    mock_engines::Uniform32 eng32;
    for (int i = 0; i < N; ++i) {
        f64 val = GenerateCanonical<f64>(eng32);
        ASSERT_GE(val, 0.0);
        ASSERT_LT(val, 1.0);
    }

    // --- float from 64-bit engine ---
    mock_engines::Uniform64 eng64_for_float;
    for (int i = 0; i < N; ++i) {
        float val = GenerateCanonical<float>(eng64_for_float);
        ASSERT_GE(val, 0.0f);
        ASSERT_LT(val, 1.0f);
    }

    // --- float from 32-bit engine ---
    mock_engines::Uniform32 eng32_for_float;
    for (int i = 0; i < N; ++i) {
        float val = GenerateCanonical<float>(eng32_for_float);
        ASSERT_GE(val, 0.0f);
        ASSERT_LT(val, 1.0f);
    }
}

TEST(GenerateCanonical, GenericPathFallback)
{
    constexpr int N = 1000;

    // --- 非位均匀引擎 ---
    mock_engines::NonUniform non_uniform_eng;
    for (int i = 0; i < N; ++i) {
        f64 val = GenerateCanonical<f64>(non_uniform_eng);
        ASSERT_GE(val, 0.0);
        ASSERT_LT(val, 1.0);
    }

    // --- 引擎位数不足的组合 ---
    mock_engines::Uniform16 eng16;
    for (int i = 0; i < N; ++i) {
        f64 val = GenerateCanonical<f64>(eng16);
        ASSERT_GE(val, 0.0);
        ASSERT_LT(val, 1.0);
    }
}

// ===================================================================
// 伪性能测试
// ===================================================================

TEST(GenerateCanonical, EfficiencySmokeTest)
{
    constexpr long long N_CALLS = 1000000; // 一百万次调用

    mock_engines::Uniform64 fast_eng;
    auto start_fast = std::chrono::high_resolution_clock::now();
    volatile f64 sink_fast = 0; // volatile 防止编译器优化掉整个循环
    for (long long i = 0; i < N_CALLS; ++i) { sink_fast += GenerateCanonical<f64>(fast_eng); }
    auto end_fast = std::chrono::high_resolution_clock::now();
    std::chrono::duration<f64, std::milli> duration_fast = end_fast - start_fast;

    std::cout << "[ PERF ] Fast Path (f64 from 64-bit) took: " << duration_fast.count() << " ms\n";

    mock_engines::Uniform16 slow_eng;
    auto start_slow = std::chrono::high_resolution_clock::now();
    volatile f64 sink_slow = 0;
    for (long long i = 0; i < N_CALLS; ++i) { sink_slow += GenerateCanonical<f64>(slow_eng); }
    auto end_slow = std::chrono::high_resolution_clock::now();
    std::chrono::duration<f64, std::milli> duration_slow = end_slow - start_slow;

    std::cout << "[ PERF ] Generic Path (f64 from 16-bit) took: " << duration_slow.count() << " ms\n";

    // 快速路径的耗时应该显著小于通用路径。
    EXPECT_LT(duration_fast.count(), duration_slow.count());
}

// ==============================================================
// 便捷 API 测试
// ==============================================================

TEST(UniformAPIs, IntRange)
{
    constexpr int MINV = -10;
    constexpr int MAXV = 25;

    for (int i = 0; i < 1000; ++i) {
        int v = Uniform<int>(MINV, MAXV);
        ASSERT_GE(v, MINV);
        ASSERT_LE(v, MAXV);
    }
}

TEST(UniformAPIs, FloatRange)
{
    for (int i = 0; i < 1000; ++i) {
        double u = uniform01<double>();
        ASSERT_GE(u, 0.0);
        ASSERT_LT(u, 1.0);

        float f = Uniform<float>(-2.5f, 3.5f);
        ASSERT_GE(f, -2.5f);
        ASSERT_LT(f, 3.5f);
    }
}

TEST(ShuffleAPI, FisherYatesPermutation)
{
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7};
    auto origin = data;
    Shuffle(data);

    // 打乱后应依旧是一个排列
    auto s1 = data;
    auto s2 = origin;
    std::sort(s1.begin(), s1.end());
    std::sort(s2.begin(), s2.end());
    ASSERT_EQ(s1, s2);
}

TEST(PCG32Advance, MatchesManualDiscard)
{
    // 验证 advance(n) 与手动丢弃 n 次的结果一致
    PCG32Engine engA(123456u);
    PCG32Engine engB(123456u);

    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) { (void)engA(); }

    engB.advance(N);

    auto a = engA();
    auto b = engB();
    ASSERT_EQ(a, b);
}

namespace {
struct uint128_tt
{
    u64 low;
    u64 high;

    constexpr explicit uint128_tt(u64 l = 0, u64 h = 0) noexcept : low(l), high(h) {}

    constexpr uint128_tt& operator*=(const uint128_tt& other) noexcept
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


using u128t1 = __uint128_t;
using u128t2 = uint128_tt;

constexpr u64 GetLow64(const u128t1& v) { return static_cast<u64>(v); }

constexpr u64 GetLow64(const u128t2& v) { return static_cast<u64>(v); }

constexpr u64 GetHigh64(const u128t1& v) { return static_cast<u64>(v >> 64); }

constexpr u64 GetHigh64(const u128t2& v) { return v.high; }

constexpr u128t1 MakeU128t1(u64 low, u64 high) { return (static_cast<u128>(high) << 64) | low; }

constexpr u128t2 MakeU128t2(u64 low, u64 high) { return u128t2(low, high); }
}

TEST(Uint128t1, Construction)
{
    constexpr u128t1 zero{};
    EXPECT_EQ(GetLow64(zero), 0);
    EXPECT_EQ(GetHigh64(zero), 0);

    constexpr u128t1 from_u64(0x123456789ABCDEF0ULL);
    EXPECT_EQ(GetLow64(from_u64), 0x123456789ABCDEF0ULL);
    EXPECT_EQ(GetHigh64(from_u64), 0);
}

TEST(Uint128t1, MultiplicationAssignment)
{
    using u128 = u128t1;

    // 1. 乘以 0
    u128 val_a = MakeU128t1(12345, 67890);
    val_a *= u128(0);
    EXPECT_EQ(GetLow64(val_a), 0);
    EXPECT_EQ(GetHigh64(val_a), 0);

    // 2. 乘以 1
    u128 val_b = MakeU128t1(0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL);
    u128 val_b_copy = val_b;
    val_b *= u128(1);
    EXPECT_EQ(GetLow64(val_b), GetLow64(val_b_copy));
    EXPECT_EQ(GetHigh64(val_b), GetHigh64(val_b_copy));

    // 3. 简单乘法，不溢出到高位
    u128 val_c(1ULL << 16);
    val_c *= u128(1ULL << 16);
    EXPECT_EQ(GetLow64(val_c), 1ULL << 32);
    EXPECT_EQ(GetHigh64(val_c), 0);

    // 4. 刚好溢出到高位的乘法 (2^32 * 2^32 = 2^64)
    u128 val_d(1ULL << 32);
    val_d *= u128(1ULL << 32);
    EXPECT_EQ(GetLow64(val_d), 0);
    EXPECT_EQ(GetHigh64(val_d), 1);

    // 5. 一个更复杂的乘法
    // (2^48 + 1) * (2^48 + 1) = 2^96 + 2*2^48 + 1
    // high = 2^32, low = 2^49 + 1
    u128 val_e((1ULL << 48) + 1);
    val_e *= u128((1ULL << 48) + 1);
    EXPECT_EQ(GetLow64(val_e), (1ULL << 49) + 1);
    EXPECT_EQ(GetHigh64(val_e), (1ULL << 32));
}

TEST(Uint128t2, Construction)
{
    constexpr u128t2 zero;
    EXPECT_EQ(GetLow64(zero), 0);
    EXPECT_EQ(GetHigh64(zero), 0);

    constexpr u128t2 from_u64(0x123456789ABCDEF0ULL);
    EXPECT_EQ(GetLow64(from_u64), 0x123456789ABCDEF0ULL);
    EXPECT_EQ(GetHigh64(from_u64), 0);
}

TEST(Uint128t2, MultiplicationAssignment)
{
    using u128 = u128t2;
    // 1. 乘以 0
    u128 val_a = MakeU128t2(12345, 67890);
    val_a *= u128(0);
    EXPECT_EQ(GetLow64(val_a), 0);
    EXPECT_EQ(GetHigh64(val_a), 0);

    // 2. 乘以 1
    u128 val_b = MakeU128t2(0xAAAAAAAAAAAAAAAAULL, 0x5555555555555555ULL);
    u128 val_b_copy = val_b;
    val_b *= u128(1);
    EXPECT_EQ(GetLow64(val_b), GetLow64(val_b_copy));
    EXPECT_EQ(GetHigh64(val_b), GetHigh64(val_b_copy));

    // 3. 简单乘法，不溢出到高位
    u128 val_c(1ULL << 16);
    val_c *= u128(1ULL << 16);
    EXPECT_EQ(GetLow64(val_c), 1ULL << 32);
    EXPECT_EQ(GetHigh64(val_c), 0);

    // 4. 刚好溢出到高位的乘法 (2^32 * 2^32 = 2^64)
    u128 val_d(1ULL << 32);
    val_d *= u128(1ULL << 32);
    EXPECT_EQ(GetLow64(val_d), 0);
    EXPECT_EQ(GetHigh64(val_d), 1);

    // 5. 一个更复杂的乘法
    // (2^48 + 1) * (2^48 + 1) = 2^96 + 2*2^48 + 1
    // high = 2^32, low = 2^49 + 1
    u128 val_e((1ULL << 48) + 1);
    val_e *= u128((1ULL << 48) + 1);
    EXPECT_EQ(GetLow64(val_e), (1ULL << 49) + 1);
    EXPECT_EQ(GetHigh64(val_e), (1ULL << 32));
}