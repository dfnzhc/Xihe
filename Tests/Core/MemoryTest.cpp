/**
 * @File MemoryTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Memory/Allocators.hpp>
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
    auto src = MakeCpuMemorySource(cap, alg);
    auto* base = static_cast<std::byte*>(src->map());
    ASSERT_NE(base, nullptr);
    // 对齐检查
    EXPECT_EQ(reinterpret_cast<uintptr_t>(base) % alg, 0u);
    // 写入边界检查（首尾字节可访问）
    base[0] = std::byte{0xAB};
    base[cap - 1] = std::byte{0xCD};
    EXPECT_EQ(base[0], std::byte{0xAB});
    EXPECT_EQ(base[cap - 1], std::byte{0xCD});
}

// ---------------- PoolAllocator Tests ----------------

TEST(PoolAllocator, FixedBlocksAllocateDeallocate)
{
    constexpr Size kBlock = 64;
    constexpr Size kCount = 128;
    PoolAllocator pool(kBlock, kCount);

    std::vector<AllocationHandle> hs;
    hs.reserve(kCount);
    for (Size i = 0; i < kCount; ++i) {
        auto h = pool.allocate(32, 8);
        ASSERT_TRUE(h);
        // 对齐至少满足 max_align
        EXPECT_EQ(reinterpret_cast<uintptr_t>(h.cpuPtr) % alignof(std::max_align_t), 0u);
        hs.push_back(h);
    }
    // 超额应扩容或继续可用；当前实现会扩容一次
    auto extra = pool.allocate(16, 8);
    ASSERT_TRUE(extra);
    // 释放全部
    for (auto& h : hs)
        pool.deallocate(h);
    pool.deallocate(extra);
    EXPECT_EQ(pool.stats().bytesInUse.load(), 0u);
}

TEST(PoolAllocator, RejectTooLargeRequest)
{
    PoolAllocator pool(64, 16);
    auto tooBig = pool.allocate(128, 8);
    EXPECT_FALSE(tooBig);
}

TEST(PoolAllocator, ConcurrencyBasic)
{
    PoolAllocator pool(64, 64);
    constexpr Size N = 4;
    constexpr Size K = 1000;
    std::vector<std::thread> ts;
    std::atomic<Size> ok{0};
    for (Size t = 0; t < N; ++t) {
        ts.emplace_back([&] {
            std::vector<AllocationHandle> hs;
            hs.reserve(K);
            for (Size i = 0; i < K; ++i)
                hs.push_back(pool.allocate(16, 8));
            for (auto& h : hs)
                ASSERT_TRUE(h);
            for (auto& h : hs)
                pool.deallocate(h);
            ok.fetch_add(1);
        });
    }
    for (auto& th : ts)
        th.join();
    EXPECT_EQ(ok.load(), N);
    EXPECT_EQ(pool.stats().bytesInUse.load(), 0u);
}

// ---------------- GeneralPurposeAllocator Tests ----------------

TEST(GeneralPurposeAllocator, SmallGoesToPool)
{
    GeneralPurposeAllocator gp(512);
    auto s1 = gp.allocate(32, 8);
    auto s2 = gp.allocate(256, 16);
    ASSERT_TRUE(s1 && s2);
    // gp 自身统计不应增长（小块在内置池中计）
    EXPECT_EQ(gp.stats().numAllocations.load(), 0u);
    EXPECT_EQ(gp.stats().bytesInUse.load(), 0u);
    gp.deallocate(s1);
    gp.deallocate(s2);
}

TEST(GeneralPurposeAllocator, LargeGoesToSystem)
{
    GeneralPurposeAllocator gp(256);
    auto big = gp.allocate(4096, 64);
    ASSERT_TRUE(big);
    EXPECT_EQ(gp.stats().numAllocations.load(), 1u);
    EXPECT_EQ(gp.stats().bytesInUse.load(), 4096u);
    gp.deallocate(big);
    EXPECT_EQ(gp.stats().bytesInUse.load(), 0u);
}

TEST(GeneralPurposeAllocator, AlignmentRespected)
{
    GeneralPurposeAllocator gp(128);
    auto h = gp.allocate(1024, 256);
    ASSERT_TRUE(h);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(h.cpuPtr) % 256, 0u);
    gp.deallocate(h);
}

// ---------------- Provider-enabled Tests ----------------

namespace {
class TestCountingProvider : public IBlockProvider
{
public:
    void* allocateBlock(Size bytes, Size alignment) override
    {
        ++allocCalls;
        lastBytes = bytes;
        lastAlignment = alignment;
        return mi_malloc_aligned(bytes, alignment);
    }

    void freeBlock(void* base, Size bytes) override
    {
        ++freeCalls;
        freedBytes += bytes;
        mi_free(base);
    }

    Size allocCalls{0};
    Size freeCalls{0};
    Size lastBytes{0};
    Size lastAlignment{0};
    Size freedBytes{0};
};
} // namespace

TEST(PoolAllocatorWithProvider, ExpandUsesProviderAndFreeBack)
{
    TestCountingProvider provider;
    constexpr Size block = 64;
    constexpr Size perChunk = 32;
    {
        PoolAllocator pool(block, perChunk, &provider);
        // 触发至少一次扩容
        std::vector<AllocationHandle> hs;
        hs.reserve(perChunk + 1);
        for (Size i = 0; i < perChunk + 1; ++i)
            hs.push_back(pool.allocate(32, 8));
        for (auto& h : hs)
            ASSERT_TRUE(h);
        // provider 应至少被调用一次，且块大小匹配
        EXPECT_GE(provider.allocCalls, 1u);
        EXPECT_EQ(provider.lastBytes, block * perChunk);
        EXPECT_EQ(provider.lastAlignment, alignof(std::max_align_t));
        // 析构 pool 触发归还（作用域结束）
    }
    EXPECT_GE(provider.freeCalls, 1u);
    EXPECT_GE(provider.freedBytes, block * perChunk);
}

TEST(GeneralPurposeAllocatorWithProvider, LargeGoesThroughProvider)
{
    TestCountingProvider provider;
    // 小块阈值设置较小，保证 4KB 走大块路径
    GeneralPurposeAllocator gp(128, &provider);
    auto big = gp.allocate(4096, 256);
    ASSERT_TRUE(big);
    EXPECT_GE(provider.allocCalls, 1u);
    EXPECT_EQ(provider.lastBytes, 4096u);
    EXPECT_EQ(provider.lastAlignment, 256u);
    gp.deallocate(big);
    EXPECT_GE(provider.freeCalls, 1u);
    EXPECT_GE(provider.freedBytes, 4096u);
}


TEST(RingAllocator, BasicAllocateAndFreeFrontNoWrap)
{
    constexpr Size cap = 1024;
    RingAllocator ring(cap);

    auto a = ring.allocate(256, 16);
    ASSERT_TRUE(a);
    EXPECT_EQ(a.offset % 16, 0u);

    auto b = ring.allocate(128, 32);
    ASSERT_TRUE(b);
    EXPECT_EQ(b.offset % 32, 0u);

    // stats bytesInUse 按保留字节（含对齐/填充）累计，但此处无跨尾，接近请求和
    auto before = ring.stats().bytesInUse.load();
    EXPECT_GE(before, 256u + 128u);

    ring.freeFront(256);
    auto mid = ring.stats().bytesInUse.load();
    EXPECT_LE(mid, before);

    ring.freeFront(128);
    EXPECT_EQ(ring.stats().bytesInUse.load(), 0u);
}

TEST(RingAllocator, WrapWithPaddingAndFreeFront)
{
    // 先分配 200，使 head 落在尾部附近；释放后 head 保持在 200，tail 回到 head
    // 再分配 64，将从 200 对齐后跨尾，触发 padding，最终 offset 应为 0
    RingAllocator ring(256);

    auto a = ring.allocate(200, 16);
    ASSERT_TRUE(a);
    // 顺序释放第一段，使可用空间恢复，但 head 仍在 200
    ring.freeFront(200);

    auto b = ring.allocate(64, 16);
    ASSERT_TRUE(b);
    EXPECT_EQ(b.offset, 0u);

    // 第二段保留大小应为 56(padding)+64=120，bytesInUse 至少达到 120
    EXPECT_GE(ring.stats().bytesInUse.load(), 120u);
    ring.freeFront(120);
    EXPECT_EQ(ring.stats().bytesInUse.load(), 0u);
}

TEST(RingAllocator, FailWhenInsufficient)
{
    RingAllocator ring(128);
    auto a = ring.allocate(120, 16);
    ASSERT_TRUE(a);
    // 此时可用仅 8，若再请求 16 且需跨尾 padding=8，总保留=24>8，应失败
    auto b = ring.allocate(16, 16);
    EXPECT_FALSE(b);

    // 释放后应可再次分配
    ring.freeFront(120);
    auto c = ring.allocate(64, 16);
    EXPECT_TRUE(c);
}

// ---------------- ScopeAllocator Tests ----------------

TEST(ScopeAllocator, SingleScopeAllocateCloseComplete)
{
    auto src = MakeCpuMemorySource(1_MiB);
    ScopeAllocator sa(src);

    auto scope = sa.BeginScope();
    std::vector<AllocationHandle> hs;
    for (int i = 0; i < 128; ++i) {
        auto h = scope.Allocate(1024, 16);
        ASSERT_TRUE(h);
        hs.push_back(h);
    }
    auto ticket = scope.Close();
    ASSERT_TRUE(ticket.isValid());
    sa.Complete(ticket);
    EXPECT_EQ(sa.stats().bytesInUse.load(), 0u);
}

TEST(ScopeAllocator, OutOfOrderCompleteIsQueued)
{
    auto src = MakeCpuMemorySource(64_KiB);
    ScopeAllocator sa(src);

    auto s1 = sa.BeginScope();
    auto h1 = s1.Allocate(8_KiB, 16);
    ASSERT_TRUE(h1);
    auto t1 = s1.Close();

    auto s2 = sa.BeginScope();
    auto h2 = s2.Allocate(4_KiB, 16);
    ASSERT_TRUE(h2);
    auto t2 = s2.Close();

    sa.Complete(t2); // out-of-order，不应清零
    EXPECT_GT(sa.stats().bytesInUse.load(), 0u);

    sa.Complete(t1); // 推进到 t1，并继续推进已完成的 t2
    EXPECT_EQ(sa.stats().bytesInUse.load(), 0u);
}

TEST(ScopeAllocator, MultiThreadAllocateWithinOneScope)
{
    auto src = MakeCpuMemorySource(2_MiB);
    ScopeAllocator sa(src);
    auto scope = sa.BeginScope();

    constexpr int Threads = 4;
    constexpr int PerThreadAllocs = 256;
    std::vector<std::thread> ts;
    ts.reserve(Threads);
    std::atomic<int> ok{0};
    for (int t = 0; t < Threads; ++t) {
        ts.emplace_back([&] {
            for (int i = 0; i < PerThreadAllocs; ++i) {
                auto h = scope.Allocate(64, 16);
                ASSERT_TRUE(h);
            }
            ok.fetch_add(1);
        });
    }
    for (auto& th : ts) th.join();
    EXPECT_EQ(ok.load(), Threads);
    auto ticket = scope.Close();
    sa.Complete(ticket);
    EXPECT_EQ(sa.stats().bytesInUse.load(), 0u);
}