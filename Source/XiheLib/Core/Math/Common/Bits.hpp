/**
 * @File Bits.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/4
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Math/Common/Basic.hpp"
#include <bit>

namespace xihe {
template<typename T>
constexpr int BitSize()
{
    static_assert(std::is_unsigned_v<T>, "类型 T 需要是无符号整型.");
    return std::numeric_limits<T>::digits;
}

template<cUnsignedType T>
constexpr int CountLeadingZeros(T value) { return std::countl_zero(value); }

template<cUnsignedType T>
constexpr int CountTrailingZeros(T value) { return std::countr_zero(value); }

template<cUnsignedType T>
constexpr int Popcount(T value) { return std::popcount(value); }

template<cUnsignedType T>
constexpr int CountSetBits(T value) { return Popcount(value); }

template<cUnsignedType T>
constexpr int CountClearedBits(T value) { return BitSize<T>() - Popcount(value); }

template<cUnsignedType T>
constexpr int Parity(T value) { return Popcount(value) % 2; }

template<cUnsignedType T>
constexpr bool HasSingleBit(T value) { return std::has_single_bit(value); }

template<cUnsignedType T>
constexpr bool IsPowerOfTwo(T value) { return HasSingleBit(value); }

template<cUnsignedType T>
constexpr T BitFloor(T value) { return std::bit_floor(value); }

template<cUnsignedType T>
constexpr T PreviousPowerOfTwo(T value) { return BitFloor(value); }

template<cUnsignedType T>
constexpr int FloorLog2(T value)
{
    if (value == 0)
        return -1;
    return BitSize<T>() - 1 - CountLeadingZeros(value);
}

template<cUnsignedType T>
constexpr int BitWidth(T value) { return std::bit_width(value); }

template<cUnsignedType T>
constexpr T BitCeil(T value) { return std::bit_ceil(value); }

template<cUnsignedType T>
constexpr T NextPowerOfTwo(T value) { return BitCeil(value); }

template<cUnsignedType T>
constexpr T ClosestPowerOfTwo(T value)
{
    if (value == 0)
        return 1;

    auto nx = NextPowerOfTwo(value);
    auto px = PreviousPowerOfTwo(value);
    return (nx - value) >= (value - px) ? px : nx;
}

template<cUnsignedType T>
constexpr T ReverseBits(T value)
{
    T v = value;
    if constexpr (sizeof(T) == 1) {
        // 8-bit
        v = ((v & 0xF0) >> 4) | ((v & 0x0F) << 4);
        v = ((v & 0xCC) >> 2) | ((v & 0x33) << 2);
        v = ((v & 0xAA) >> 1) | ((v & 0x55) << 1);
    }
    else if constexpr (sizeof(T) == 2) {
        // 16-bit
        v = ((v & 0xFF00) >> 8) | ((v & 0x00FF) << 8);
        v = ((v & 0xF0F0) >> 4) | ((v & 0x0F0F) << 4);
        v = ((v & 0xCCCC) >> 2) | ((v & 0x3333) << 2);
        v = ((v & 0xAAAA) >> 1) | ((v & 0x5555) << 1);
    }
    else if constexpr (sizeof(T) == 4) {
        // 32-bit
        v = ((v & 0xFFFF0000) >> 16) | ((v & 0x0000FFFF) << 16);
        v = ((v & 0xFF00FF00) >> 8) | ((v & 0x00FF00FF) << 8);
        v = ((v & 0xF0F0F0F0) >> 4) | ((v & 0x0F0F0F0F) << 4);
        v = ((v & 0xCCCCCCCC) >> 2) | ((v & 0x33333333) << 2);
        v = ((v & 0xAAAAAAAA) >> 1) | ((v & 0x55555555) << 1);
    }
    else if constexpr (sizeof(T) == 8) {
        // 64-bit
        v = ((v & 0xFFFFFFFF00000000) >> 32) | ((v & 0x00000000FFFFFFFF) << 32);
        v = ((v & 0xFFFF0000FFFF0000) >> 16) | ((v & 0x0000FFFF0000FFFF) << 16);
        v = ((v & 0xFF00FF00FF00FF00) >> 8) | ((v & 0x00FF00FF00FF00FF) << 8);
        v = ((v & 0xF0F0F0F0F0F0F0F0) >> 4) | ((v & 0x0F0F0F0F0F0F0F0F) << 4);
        v = ((v & 0xCCCCCCCCCCCCCCCC) >> 2) | ((v & 0x3333333333333333) << 2);
        v = ((v & 0xAAAAAAAAAAAAAAAA) >> 1) | ((v & 0x5555555555555555) << 1);
    }

    return v;
}

template<std::unsigned_integral T>
constexpr T BitSwap(T value)
{
    T v = value;
    if constexpr (sizeof(T) == 1) { return v; }
    else if constexpr (sizeof(T) == 2) { return (v >> 8) | (v << 8); }
    else if constexpr (sizeof(T) == 4) { return ((v << 24) | ((v << 8) & 0x00FF0000) | ((v >> 8) & 0x0000FF00) | (v >> 24)); }
    else {
        v = (v & 0x00000000FFFFFFFF) << 32 | (v & 0xFFFFFFFF00000000) >> 32;
        v = (v & 0x0000FFFF0000FFFF) << 16 | (v & 0xFFFF0000FFFF0000) >> 16;
        v = (v & 0x00FF00FF00FF00FF) << 8 | (v & 0xFF00FF00FF00FF00) >> 8;
        return v;
    }
}

template<cUnsignedType T>
constexpr T RotateLeft(T value, int count) { return std::rotl(value, count); }

template<cUnsignedType T>
constexpr T RotateRight(T value, int count) { return std::rotr(value, count); }

template<cUnsignedType T>
constexpr T SetBit(T value, int pos) { return value | (static_cast<T>(1) << pos); }

template<cUnsignedType T>
constexpr T ClearBit(T value, int pos) { return value & ~(static_cast<T>(1) << pos); }

template<cUnsignedType T>
constexpr T ToggleBit(T value, int pos) { return value ^ (static_cast<T>(1) << pos); }

template<cUnsignedType T>
constexpr bool CheckBit(T value, int pos) { return (value >> pos) & 1; }

constexpr u32 RoundUp(u32 x, u32 y)
{
    if (x == 0)
        return y;
    return ((x + y - 1) / y) * y;
}

constexpr Size AlignUp(Size value, Size alignment)
{
    // Assumes alignment is a power of two
    return (value + alignment - 1) & ~(alignment - 1);
}

template<cUnsignedType T>
constexpr T UintMinus(T value) noexcept { return (value == std::numeric_limits<T>::min()) ? std::numeric_limits<T>::max() : value - 1; }

constexpr u64 MergeUint32ToUint64(u32 high, u32 low) noexcept { return (As<u64>(high) << 32) | low; }

template<cUnsignedType T>
constexpr u32 CrushToUint32(T value)
{
    if constexpr (sizeof(value) <= 4) { return As<u32>(value); }
    else {
        const auto res = As<u64>(value) * 0xbc2ad017d719504d;
        return As<u32>(res ^ (res >> 32));
    }
}
} // namespace xihe