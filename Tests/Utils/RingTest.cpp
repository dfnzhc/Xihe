/**
 * @File RingTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/10
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Utils/Ring.hpp>

using namespace xihe;

TEST(RingCore, ReserveWithinCapacity)
{
    Ring ring(1024);
    Ring::ReserveResult r{};
    ASSERT_TRUE(ring.tryReserve(128, 16, r));
    EXPECT_TRUE(r.ok);
    EXPECT_EQ(r.reservedBytes, 128u);
    EXPECT_EQ(r.paddingBytes, 0u);
    EXPECT_LT(r.finalOffset, 1024u);
    EXPECT_GT(r.endCounter, 0u);
}

TEST(RingCore, WrapWithPadding)
{
    Ring ring(256);
    Ring::ReserveResult r1{}, r2{};
    ASSERT_TRUE(ring.tryReserve(200, 16, r1));
    // 释放前一段，使下次从 200 起点触发跨尾
    ring.setTail(r1.endCounter);
    // 触发跨尾
    ASSERT_TRUE(ring.tryReserve(128, 16, r2));
    EXPECT_GT(r2.paddingBytes, 0u);
    EXPECT_LT(r2.finalOffset, 256u);
}

TEST(RingCore, FailWhenInsufficient)
{
    Ring ring(128);
    Ring::ReserveResult r{};
    ASSERT_TRUE(ring.tryReserve(120, 16, r));
    Ring::ReserveResult r2{};
    // 可用不足时失败
    EXPECT_FALSE(ring.tryReserve(64, 16, r2));
}

TEST(RingCore, SetTailAndBytesInUse)
{
    Ring ring(512);
    Ring::ReserveResult r{};
    ASSERT_TRUE(ring.tryReserve(128, 16, r));
    auto used1 = ring.bytesInUse();
    EXPECT_GE(used1, 128u);
    // 推进 tail
    ring.setTail(r.endCounter);
    EXPECT_EQ(ring.bytesInUse(), 0u);
}

TEST(RingCore, FreeFront)
{
    Ring ring(512);
    Ring::ReserveResult r{};
    ASSERT_TRUE(ring.tryReserve(256, 16, r));
    auto used1 = ring.bytesInUse();
    ring.freeFront(128);
    auto used2 = ring.bytesInUse();
    EXPECT_LT(used2, used1);
}
