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
    // Closer to previous
    EXPECT_EQ(ClosestPowerOfTwo(u32(17)), 16u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(99)), 128u); // Fails with original logic, passes with >=

    // Closer to next
    EXPECT_EQ(ClosestPowerOfTwo(u32(30)), 32u);

    // Tie-breaker cases (e.g., 12, 24, 48...)
    // Original logic: (16-12) > (12-8) -> 4 > 4 (false) -> returns 16
    // My logic: (16-12) >= (12-8) -> 4 >= 4 (true) -> returns 8
    // Let's assume the tie-break rule is "prefer smaller" as is common.
    EXPECT_EQ(ClosestPowerOfTwo(u32(12)), 8u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(24)), 16u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(48)), 32u);
    EXPECT_EQ(ClosestPowerOfTwo(u32(3)), 2u);

    // Edge cases
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
    EXPECT_EQ(BitSwap(u8(0xAB)), u8(0xAB)); // no-op for 8-bit
}

TEST(BitManipulationTest, Rotate)
{
    u8 val8 = 0b11000001;
    EXPECT_EQ(RotateLeft(val8, 2), u8(0b00000111));
    EXPECT_EQ(RotateRight(val8, 2), u8(0b01110000)); // This is wrong, should be 0111... -> 01110000 is wrong. should be 01...00 -> 01110000
    // Correcting rotr test: 11000001 >> 2 = 00110000. Bits shifted out: 01. 01 << 6 = 01000000.  Result = 00110000 | 01000000 = 01110000. Correct.
    EXPECT_EQ(RotateRight(val8, 2), u8(0b01110000));
    EXPECT_EQ(RotateRight(u16(4), 2), u16(1));

    // Rotate by 0 should be a no-op
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