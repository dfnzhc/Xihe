/**
 * @File CommonTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/5
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <Core/Math/Common.hpp>

using namespace xihe;

TEST(MathFunctionsTest, MinMax)
{
    EXPECT_EQ(Min(5, 10), 5);
    EXPECT_EQ(Max(5, 10), 10);

    EXPECT_EQ(Min(5, 1, 10, -3, 8), -3);
    EXPECT_EQ(Max(5, 1, 10, -3, 8), 10);

    EXPECT_FLOAT_EQ(Min(3.14f, 2.71f), 2.71f);
    EXPECT_DOUBLE_EQ(Max(-10.5, -20.5), -10.5);

    EXPECT_DOUBLE_EQ(Min(1.0, 2.0, 3.0, 0.5, 4.0), 0.5);
    EXPECT_DOUBLE_EQ(Max(1.0, 2.0, 3.0, 0.5, 4.0), 4.0);
}

TEST(MathFunctionsTest, Equality)
{
    EXPECT_TRUE(Equal(10, 10));
    EXPECT_FALSE(Equal(10, 11));
    EXPECT_TRUE(NotEqual(10, 11));
    EXPECT_FALSE(NotEqual(10, 10));

    EXPECT_TRUE(Equal(5.0f, 5.0f));
    EXPECT_FALSE(Equal(5.0f, 5.000001f));
}

TEST(MathFunctionsTest, FusedMultiplyAdd)
{
    EXPECT_FLOAT_EQ(FMA(2.0f, 3.0f, 4.0f), 10.0f);
    EXPECT_DOUBLE_EQ(FMA(10.0, 2.5, -5.0), 20.0);
}

TEST(MathFunctionsTest, AngleConversions)
{
    const double tolerance = 1e-9;
    // --- 测试 Radians ---
    EXPECT_NEAR(Radians(180.0), kPi, tolerance);
    EXPECT_NEAR(Radians(90), kPi / 2.0, tolerance);
    EXPECT_NEAR(Radians(0.0f), 0.0, tolerance);

    // --- 测试 Degrees ---
    EXPECT_NEAR(Degrees(kPi), 180.0, tolerance);
    EXPECT_NEAR(Degrees(kPi / 4.0), 45.0, tolerance);
    EXPECT_NEAR(Degrees(0.0f), 0.0, tolerance);
}

TEST(MathFunctionsTest, PowerFunctions)
{
    // --- 测试 Pow<T, N> ---
    // N = 0
    EXPECT_EQ((Pow<int, 0>(123)), 1);
    // N = 1
    EXPECT_EQ((Pow<int, 1>(123)), 123);
    // N > 1 (偶数)
    EXPECT_EQ((Pow<int, 4>(2)), 16);
    // N > 1 (奇数)
    EXPECT_EQ((Pow<int, 3>(-3)), -27);
    // N < 0
    EXPECT_DOUBLE_EQ((Pow<double, -2>(4.0)), 0.0625); // 1.0 / 16.0

    static_assert(Pow<int, 10>(2) == 1024, "Pow should be constexpr");
    static_assert(Pow<long long, 3>(10) == 1000, "Pow should be constexpr");

    // --- 测试 Pow2 ---
    EXPECT_EQ(Pow2(5), 25);
    EXPECT_FLOAT_EQ(Pow2(1.5f), 2.25f);
    static_assert(Pow2(12) == 144, "Pow2 should be constexpr");
}

TEST(ClampFunctionsTest, GeneralClamp)
{
    // --- 测试整数类型 (int) ---
    // 值在范围内
    EXPECT_EQ(Clamp(5, 0, 10), 5);
    // 值低于下限
    EXPECT_EQ(Clamp(-5, 0, 10), 0);
    // 值高于上限
    EXPECT_EQ(Clamp(15, 0, 10), 10);

    // 测试负数范围
    EXPECT_EQ(Clamp(-15, -20, -10), -15);
    EXPECT_EQ(Clamp(-25, -20, -10), -20);
    EXPECT_EQ(Clamp(-5, -20, -10), -10);

    // 测试边界值
    EXPECT_EQ(Clamp(0, 0, 10), 0);
    EXPECT_EQ(Clamp(10, 0, 10), 10);

    // --- 测试浮点数类型 (float, double) ---
    // 值在范围内
    EXPECT_FLOAT_EQ(Clamp(0.5f, 0.0f, 1.0f), 0.5f);
    // 值低于下限
    EXPECT_FLOAT_EQ(Clamp(-0.5f, 0.0f, 1.0f), 0.0f);
    // 值高于上限
    EXPECT_FLOAT_EQ(Clamp(1.5f, 0.0f, 1.0f), 1.0f);

    // 测试 double 和负数范围
    EXPECT_DOUBLE_EQ(Clamp(-15.5, -20.0, -10.0), -15.5);
}

TEST(ClampFunctionsTest, ClampHighToZero)
{
    // --- 测试整数类型 (int) ---
    // 值在 [0, hi] 范围内
    EXPECT_EQ(ClampHigh(5, 10), 5);
    // 值低于 0
    EXPECT_EQ(ClampHigh(-5, 10), 0);
    // 值高于上限
    EXPECT_EQ(ClampHigh(15, 10), 10);

    // 测试边界值
    EXPECT_EQ(ClampHigh(0, 10), 0);
    EXPECT_EQ(ClampHigh(10, 10), 10);

    // --- 测试浮点数类型 (double) ---
    // 值在范围内
    EXPECT_DOUBLE_EQ(ClampHigh(50.5, 100.0), 50.5);
    // 值低于 0
    EXPECT_DOUBLE_EQ(ClampHigh(-10.0, 100.0), 0.0);
    // 值高于上限
    EXPECT_DOUBLE_EQ(ClampHigh(110.0, 100.0), 100.0);
}

TEST(ClampFunctionsTest, ClampNormalRange)
{
    // --- 测试整数类型 (int) ---
    EXPECT_EQ(ClampNormal(-5), 0);
    EXPECT_EQ(ClampNormal(0), 0);
    EXPECT_EQ(ClampNormal(1), 1);
    EXPECT_EQ(ClampNormal(5), 1);

    // --- 测试浮点数类型 (float, double) ---
    // 值低于 0
    EXPECT_FLOAT_EQ(ClampNormal(-2.0f), 0.0f);
    // 值在 [0, 1] 范围内
    EXPECT_FLOAT_EQ(ClampNormal(0.7f), 0.7f);
    // 值高于 1
    EXPECT_FLOAT_EQ(ClampNormal(2.0f), 1.0f);

    // 测试边界值
    EXPECT_DOUBLE_EQ(ClampNormal(0.0), 0.0);
    EXPECT_DOUBLE_EQ(ClampNormal(1.0), 1.0);
}

TEST(MidPointTest, IntegralTypes)
{
    // --- 测试常规正整数 (int) ---
    EXPECT_EQ(MidPoint(2, 8), 5);
    EXPECT_EQ(MidPoint(8, 2), 5);
    // 测试向下取整
    EXPECT_EQ(MidPoint(2, 9), 5);
    EXPECT_EQ(MidPoint(9, 2), 5);

    // --- 测试无符号整数 (unsigned int) ---
    EXPECT_EQ(MidPoint(100u, 200u), 150u);

    // --- 测试负数 (int) ---
    EXPECT_EQ(MidPoint(-2, -8), -5);
    // 测试向下取整 (向负无穷方向)
    EXPECT_EQ(MidPoint(-2, -9), -6);

    // --- 测试混合符号 (int) ---
    EXPECT_EQ(MidPoint(-10, 0), -5);
    EXPECT_EQ(MidPoint(-10, 2), -4);
    EXPECT_EQ(MidPoint(-9, 2), -4); // floor((-9+2)/2) = floor(-3.5) = -4

    // --- 关键测试：防溢出 ---
    constexpr int int_max = std::numeric_limits<int>::max();
    constexpr int int_min = std::numeric_limits<int>::min();
    // (int_max + int_max - 2) / 2 -> 朴素实现会溢出
    EXPECT_EQ(MidPoint(int_max, int_max - 2), int_max - 1);
    // (int_min + int_min + 2) / 2 -> 朴素实现会溢出
    EXPECT_EQ(MidPoint(int_min, int_min + 2), int_min + 1);

    // --- 验证 constexpr 特性 ---
    static_assert(MidPoint(10, 20) == 15, "Integral MidPoint should be constexpr");
}

TEST(MidPointTest, FloatingPointTypes)
{
    // --- 测试常规浮点数 (float) ---
    EXPECT_FLOAT_EQ(MidPoint(2.0f, 8.0f), 5.0f);
    EXPECT_FLOAT_EQ(MidPoint(2.5f, 8.5f), 5.5f);

    // --- 测试负数 (double) ---
    EXPECT_DOUBLE_EQ(MidPoint(-2.0, -8.0), -5.0);

    // --- 测试混合符号 (double) ---
    EXPECT_DOUBLE_EQ(MidPoint(-10.0, 0.0), -5.0);
    EXPECT_DOUBLE_EQ(MidPoint(-10.0, 2.0), -4.0);

    // --- 关键测试：极端值 ---
    constexpr double dbl_max = std::numeric_limits<double>::max();
    constexpr double dbl_min_positive = std::numeric_limits<double>::min();

    // 两个极大值 -> 应该返回极大值，而不是 inf
    EXPECT_EQ(MidPoint(dbl_max, dbl_max), dbl_max);
    // 一个极大值和一个普通值 -> 应该返回极大值的一半
    EXPECT_DOUBLE_EQ(MidPoint(dbl_max, 0.0), dbl_max / 2.0);

    // 两个极小值（正）-> 应该返回极小值
    EXPECT_EQ(MidPoint(dbl_min_positive, dbl_min_positive), dbl_min_positive);
    // 一个极小值和一个 0 -> 应该返回极小值的一半
    EXPECT_DOUBLE_EQ(MidPoint(dbl_min_positive, 0.0), dbl_min_positive / 2.0);

    // --- 验证 constexpr 特性 ---
    static_assert(MidPoint(10.0, 20.0) == 15.0, "Floating-point MidPoint should be constexpr");
}

TEST(ApproxFunctionsTest, ApproxAndNotApprox)
{
    // 测试应该相等的情况
    EXPECT_TRUE(Approx(1.0000001f, 1.0f));
    EXPECT_FALSE(NotApprox(1.0000001f, 1.0f));

    // 测试应该不等的情况
    EXPECT_FALSE(Approx(1.001f, 1.0f));
    EXPECT_TRUE(NotApprox(1.001f, 1.0f));

    // 测试自定义 epsilon
    EXPECT_TRUE(Approx(1.1f, 1.0f, 0.2f));
    EXPECT_FALSE(Approx(1.1f, 1.0f, 0.05f));

    // 验证 constexpr 特性
    static_assert(Approx(1.0f, 1.0000001f), "Approx should be constexpr");
    static_assert(NotApprox(1.0f, 1.1f), "NotApprox should be constexpr");
}

TEST(ApproxRootFunctionsTest, ApproxSqrt)
{
    constexpr f32 tolerance = 1e-4f;

    EXPECT_NEAR(ApproxSqrt(4.0f), std::sqrt(4.0f), tolerance);
    EXPECT_NEAR(ApproxSqrt(81.0f), std::sqrt(81.0f), tolerance);
    EXPECT_NEAR(ApproxSqrt(12345.0f), std::sqrt(12345.0f), tolerance);
    EXPECT_NEAR(ApproxSqrt(0.5f), std::sqrt(0.5f), tolerance);
    // 边界情况
    EXPECT_NEAR(ApproxSqrt(0.0f), 0.0f, tolerance);
}

TEST(ApproxRootFunctionsTest, ApproxCbrt)
{
    constexpr f32 tolerance = 1e-4f;

    EXPECT_NEAR(ApproxCbrt(27.0f), std::cbrt(27.0f), tolerance);
    EXPECT_NEAR(ApproxCbrt(125.0f), std::cbrt(125.0f), tolerance);
    EXPECT_NEAR(ApproxCbrt(999.0f), std::cbrt(999.0f), tolerance);
    EXPECT_NEAR(ApproxCbrt(0.125f), std::cbrt(0.125f), tolerance);
    // 边界情况
    EXPECT_NEAR(ApproxCbrt(0.0f), 0.0f, tolerance);
}

TEST(ApproxRootFunctionsTest, InvApproxSqrt)
{
    {
        // --- 测试 float 版本 ---
        constexpr f32 tolerance_f = 1e-5f;
        const f32 val_f1 = 4.0f;
        const f32 val_f2 = 100.0f;
        const f32 val_f3 = 0.25f;

        EXPECT_NEAR(InvApproxSqrt(val_f1), 1.0f / std::sqrt(val_f1), tolerance_f);
        EXPECT_NEAR(InvApproxSqrt(val_f2), 1.0f / std::sqrt(val_f2), tolerance_f);
        EXPECT_NEAR(InvApproxSqrt(val_f3), 1.0f / std::sqrt(val_f3), tolerance_f);
    }

    {
        // --- 测试 double 版本 ---
        constexpr f64 tolerance_d = 1e-4;
        const f64 val_d1 = 9.0;
        const f64 val_d2 = 64.0;
        const f64 val_d3 = 0.01;

        EXPECT_NEAR(InvApproxSqrt(val_d1), 1.0 / std::sqrt(val_d1), tolerance_d);
        EXPECT_NEAR(InvApproxSqrt(val_d2), 1.0 / std::sqrt(val_d2), tolerance_d);
        EXPECT_NEAR(InvApproxSqrt(val_d3), 1.0 / std::sqrt(val_d3), tolerance_d);
    }
}

static_assert(BitSize<u8>() == 8);
static_assert(Popcount(u32(0b1101)) == 3);
static_assert(HasSingleBit(u64(1) << 40));
static_assert(FloorLog2(u32(1000)) == 9);
static_assert(NextPowerOfTwo(u16(100)) == 128);
static_assert(ReverseBits(u8(0b11000001)) == 0b10000011);
static_assert(BitSwap(u16(0x1234)) == 0x3412);
static_assert(RotateLeft(u8(0b10000001), 1) == 0b00000011);
static_assert(SetBit(u8(0), 7) == 0b10000000);

TEST(BitManipulationTest, BitSizeAndCounts)
{
    // BitSize
    EXPECT_EQ(BitSize<u8>(), 8);
    EXPECT_EQ(BitSize<u64>(), 64);

    // Counts
    u16 val = 0b0000'0011'0101'1000; // lz=6, tz=3, pop=5
    EXPECT_EQ(CountLeadingZeros(val), 6);
    EXPECT_EQ(CountTrailingZeros(val), 3);
    EXPECT_EQ(Popcount(val), 5);
    EXPECT_EQ(CountSetBits(val), 5);
    EXPECT_EQ(CountClearedBits(val), 16 - 5);
    EXPECT_EQ(Parity(val), 1); // Odd
    EXPECT_EQ(Parity(0b11u), 0); // Even

    // Edge cases for counts
    EXPECT_EQ(CountLeadingZeros(u32(0)), 32);
    EXPECT_EQ(CountTrailingZeros(u32(0)), 32);
    EXPECT_EQ(Popcount(u64(0)), 0);
}

TEST(BitManipulationTest, PowerOfTwo)
{
    // IsPowerOfTwo / HasSingleBit
    EXPECT_TRUE(IsPowerOfTwo(u32(1) << 20));
    EXPECT_TRUE(HasSingleBit(u8(1)));
    EXPECT_FALSE(IsPowerOfTwo(u32(3) << 10));
    EXPECT_FALSE(HasSingleBit(u32(0)));

    // PreviousPowerOfTwo / BitFloor
    EXPECT_EQ(PreviousPowerOfTwo(u32(30)), 16u);
    EXPECT_EQ(BitFloor(u32(16)), 16u);
    EXPECT_EQ(PreviousPowerOfTwo(u32(1)), 1u);
    EXPECT_EQ(BitFloor(u32(0)), 0u);

    // NextPowerOfTwo / BitCeil
    EXPECT_EQ(NextPowerOfTwo(u32(17)), 32u);
    EXPECT_EQ(BitCeil(u32(16)), 16u);
    EXPECT_EQ(NextPowerOfTwo(u32(1)), 1u);
    EXPECT_EQ(BitCeil(u32(0)), 1u); // std::bit_ceil(0) is 1
}

TEST(BitManipulationTest, LogAndWidth)
{
    // FloorLog2
    EXPECT_EQ(FloorLog2(u32(1023)), 9);
    EXPECT_EQ(FloorLog2(u32(1024)), 10);
    EXPECT_EQ(FloorLog2(u32(1)), 0);
    EXPECT_EQ(FloorLog2(u32(0)), -1);

    // BitWidth
    EXPECT_EQ(BitWidth(u32(0)), 0);
    EXPECT_EQ(BitWidth(u32(7)), 3);
    EXPECT_EQ(BitWidth(u32(8)), 4);
}

TEST(BitManipulationTest, ClosestPowerOfTwo)
{
    EXPECT_EQ(ClosestPowerOfTwo(u32(17)), 16u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(99)), 128u);

    EXPECT_EQ(ClosestPowerOfTwo(u32(30)), 32u);

    EXPECT_EQ(ClosestPowerOfTwo(u32(12)), 8u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(24)), 16u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(48)), 32u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(3)), 2u);

    EXPECT_EQ(ClosestPowerOfTwo(u32(0)), 1u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(1)), 1u);
}

TEST(BitManipulationTest, ReverseAndSwap)
{
    // ReverseBits
    EXPECT_EQ(ReverseBits(u8(0b11001010)), u8(0b01010011));
    EXPECT_EQ(ReverseBits(u32(1)), u32(1) << 31);

    // BitSwap (Endianness swap)
    EXPECT_EQ(BitSwap(u16(0x1234)), u16(0x3412));
    EXPECT_EQ(BitSwap(u32(0x12345678)), u32(0x78563412));
    EXPECT_EQ(BitSwap(u64(0x0102030405060708)), u64(0x0807060504030201));
    EXPECT_EQ(BitSwap(u8(0xAB)), u8(0xAB));
}

TEST(BitManipulationTest, Rotate)
{
    u8 val8 = 0b11000001;
    EXPECT_EQ(RotateLeft(val8, 2), u8(0b00000111));
    EXPECT_EQ(RotateRight(val8, 2), u8(0b01110000));
    EXPECT_EQ(RotateRight(val8, 2), u8(0b01110000));
    EXPECT_EQ(RotateRight(u16(4), 2), u16(1));

    EXPECT_EQ(RotateLeft(val8, 0), val8);
}

TEST(BitManipulationTest, BitwiseSetClearCheck)
{
    u32 val = 0;

    // SetBit
    val = SetBit(val, 10);
    EXPECT_EQ(val, 1u << 10);
    val = SetBit(val, 20);
    EXPECT_EQ(val, (1u << 10) | (1u << 20));

    // CheckBit
    EXPECT_TRUE(CheckBit(val, 10));
    EXPECT_TRUE(CheckBit(val, 20));
    EXPECT_FALSE(CheckBit(val, 15));

    // ClearBit
    val = ClearBit(val, 10);
    EXPECT_EQ(val, 1u << 20);
    EXPECT_FALSE(CheckBit(val, 10));

    // ToggleBit
    val = ToggleBit(val, 10); // Toggle on
    EXPECT_TRUE(CheckBit(val, 10));
    val = ToggleBit(val, 10); // Toggle off
    EXPECT_FALSE(CheckBit(val, 10));
}

TEST(UtilitiesTest, RoundUpFunction)
{
    // 常规情况
    EXPECT_EQ(RoundUp(20, 16), 32u);
    // 刚好是倍数
    EXPECT_EQ(RoundUp(32, 16), 32u);
    // 刚好在倍数的前一个数
    EXPECT_EQ(RoundUp(31, 16), 32u);
    // 刚好在倍数的后一个数
    EXPECT_EQ(RoundUp(33, 16), 48u);
    // 特殊情况：x == 0
    EXPECT_EQ(RoundUp(0, 16), 16u);

    // 验证 constexpr 特性
    static_assert(RoundUp(20, 16) == 32u, "RoundUp should be constexpr");
}

TEST(UtilitiesTest, AlignUpPowerOfTwo)
{
    // 常规情况
    EXPECT_EQ(AlignUp(20, 16), 32u);
    // 刚好对齐
    EXPECT_EQ(AlignUp(16, 16), 16u);
    // 刚好在对齐边界的前一个数
    EXPECT_EQ(AlignUp(15, 16), 16u);
    // 刚好在对齐边界的后一个数
    EXPECT_EQ(AlignUp(17, 16), 32u);
    // 值为 0
    EXPECT_EQ(AlignUp(0, 16), 0u);

    // 验证 constexpr 特性
    static_assert(AlignUp(20, 16) == 32u, "AlignUp should be constexpr");
}

TEST(UtilitiesTest, UintMinusFunction)
{
    // 常规情况
    EXPECT_EQ(UintMinus(10u), 9u);
    EXPECT_EQ(UintMinus(1u), 0u);

    // 关键测试：回绕 (wrap-around)
    EXPECT_EQ(UintMinus(0u), std::numeric_limits<unsigned int>::max());
    EXPECT_EQ(UintMinus(uint8_t{0}), std::numeric_limits<uint8_t>::max());
    EXPECT_EQ(UintMinus(uint64_t{0}), std::numeric_limits<uint64_t>::max());

    // 验证 constexpr 特性
    static_assert(UintMinus(uint16_t{0}) == std::numeric_limits<uint16_t>::max(), "UintMinus should be constexpr");
}

TEST(UtilitiesTest, MergeUint32ToUint64Function)
{
    // 常规情况
    EXPECT_EQ(MergeUint32ToUint64(0x12345678, 0x9ABCDEF0), 0x123456789ABCDEF0ULL);
    // high 为 0
    EXPECT_EQ(MergeUint32ToUint64(0, 0x9ABCDEF0), 0x000000009ABCDEF0ULL);
    // low 为 0
    EXPECT_EQ(MergeUint32ToUint64(0x12345678, 0), 0x1234567800000000ULL);
    // 全为 1
    EXPECT_EQ(MergeUint32ToUint64(0xFFFFFFFF, 0xFFFFFFFF), 0xFFFFFFFFFFFFFFFFULL);

    // 验证 constexpr 特性
    static_assert(MergeUint32ToUint64(0x1, 0x2) == 0x0000000100000002ULL, "MergeUint32ToUint64 should be constexpr");
}

TEST(UtilitiesTest, CrushToUint32Function)
{
    // 测试 sizeof <= 4 的分支 (应该为直通)
    EXPECT_EQ(CrushToUint32(uint32_t{12345}), 12345u);
    EXPECT_EQ(CrushToUint32(uint16_t{123}), 123u);

    // 测试 sizeof > 4 的分支 (哈希/混合)
    // 结果是确定的，我们把它硬编码下来以防止回归
    constexpr u64 input64 = 0x123456789ABCDEF0ULL;
    constexpr u32 expected_crushed_result = 0x5A107BEE;
    EXPECT_EQ(CrushToUint32(input64), expected_crushed_result);

    // 验证雪崩效应 (avalanche effect)：输入的小变化导致输出的大变化
    u32 crushed1 = CrushToUint32(1ULL);
    u32 crushed2 = CrushToUint32(2ULL);
    EXPECT_NE(crushed1, crushed2);
    // 两个结果应该非常不同，而不仅仅是相差 1
    EXPECT_NE(crushed2, crushed1 + 1);

    // 验证 constexpr 特性
    static_assert(CrushToUint32(uint32_t{5}) == 5u, "CrushToUint32 should be constexpr for small types");
    static_assert(CrushToUint32(input64) == expected_crushed_result, "CrushToUint32 should be constexpr for large types");
}


TEST(NumericFunctionsTest, EvaluatePolynomial)
{
    // P(t) = 2t^2 + 3t + 4

    // t = 0 -> 4
    EXPECT_DOUBLE_EQ(EvaluatePolynomial(0.0, 4, 3, 2), 4.0);
    // t = 1 -> 2 + 3 + 4 = 9
    EXPECT_DOUBLE_EQ(EvaluatePolynomial(1.0, 4, 3, 2), 9.0);
    // t = 2 -> 2*4 + 3*2 + 4 = 8 + 6 + 4 = 18
    EXPECT_DOUBLE_EQ(EvaluatePolynomial(2.0, 4, 3, 2), 18.0);
    // t = -1 -> 2 - 3 + 4 = 3
    EXPECT_DOUBLE_EQ(EvaluatePolynomial(-1.0, 4, 3, 2), 3.0);

    EXPECT_FLOAT_EQ(EvaluatePolynomial(0.5f, -5.0f, 10.0f), 0.0f);
}

TEST(NumericFunctionsTest, SumOfProducts)
{
    EXPECT_EQ(SumOfProducts(2, 3, 4, 5), 26); // (2*3) + (4*5) = 6 + 20 = 26
    EXPECT_DOUBLE_EQ(SumOfProducts(2.5, 2.0, -1.0, 5.0), 0.0); // 5.0 + (-5.0) = 0.0
    EXPECT_DOUBLE_EQ(SumOfProducts(10, 0.5, 2.5, 4), 15.0); // 5.0 + 10.0 = 15.0
}

TEST(NumericFunctionsTest, DifferenceOfProducts)
{
    EXPECT_EQ(DifferenceOfProducts(10, 5, 4, 3), 38); // (10*5) - (4*3) = 50 - 12 = 38

    EXPECT_DOUBLE_EQ(DifferenceOfProducts(10.0, 0.5, 30.0, 0.1), 2.0); // 5.0 - 3.0 = 2.0
}

TEST(NumericFunctionsTest, DifferenceOfProductsPrecisionCheck)
{
    double a = 1.0;
    double b = 1.0;
    double c = 1.0;
    double d = 1.0 - std::pow(2.0, -53);

    double exact_result = std::pow(2.0, -53);

    double naive_result = (a * b) - (c * d);

    double precise_result = DifferenceOfProducts(a, b, c, d);

    EXPECT_DOUBLE_EQ(precise_result, exact_result);
    EXPECT_DOUBLE_EQ(precise_result, naive_result);
}