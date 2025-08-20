/**
 * @File MemoryTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Memory/IAllocator.hpp>
#include <Core/Memory/Memory.hpp>

using namespace xihe;

TEST(CpuMemorySource, BasicProperties)
{
    auto src = MakeCpuMemorySource(4096, alignof(std::max_align_t));
    ASSERT_TRUE(src);
    EXPECT_EQ(src->size(), 4096u);
    EXPECT_GE(src->alignment(), alignof(std::max_align_t));
    EXPECT_EQ(src->kind(), MemorySourceKind::CPU);
}

TEST(CpuMemorySource, MapUnmapAndNativeHandle)
{
    auto src = MakeCpuMemorySource(2048);
    void* p1 = src->map();
    ASSERT_NE(p1, nullptr);
    // 返回的指针应等同于 nativeHandle（CPU 情况）
    EXPECT_EQ(p1, src->nativeHandle());
    // 多次 map 保持幂等（CPU 情况）
    void* p2 = src->map();
    EXPECT_EQ(p1, p2);
    // unmap 为 no-op（不崩、不改变句柄）
    src->unmap();
    EXPECT_EQ(p1, src->nativeHandle());
}

TEST(CpuMemorySource, AlignmentAndBounds)
{
    constexpr Size cap = 8192;
    constexpr Size alg = 64;
    auto src           = MakeCpuMemorySource(cap, alg);
    auto* base         = static_cast<std::byte*>(src->map());
    ASSERT_NE(base, nullptr);
    // 对齐检查
    EXPECT_EQ(reinterpret_cast<uintptr_t>(base) % alg, 0u);
    // 写入边界检查（首尾字节可访问）
    base[0]       = std::byte{0xAB};
    base[cap - 1] = std::byte{0xCD};
    EXPECT_EQ(base[0], std::byte{0xAB});
    EXPECT_EQ(base[cap - 1], std::byte{0xCD});
}
