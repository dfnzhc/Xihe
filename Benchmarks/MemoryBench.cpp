/**
 * @File MemoryBench.cpp
 */

#include <benchmark/benchmark.h>
#include <Core/Memory/Allocators.hpp>
#include <Core/Memory/Memory.hpp>
#include <vector>

using namespace xihe;

static void BM_Pool_MPMC(benchmark::State& st)
{
    const Size blockSize = static_cast<Size>(st.range(0));
    const Size perChunk = static_cast<Size>(st.range(1));
    const bool useProv = static_cast<bool>(st.range(2));
    CpuBlockProvider provider;
    PoolAllocator pool(blockSize, perChunk, useProv ? &provider : nullptr);
    std::vector<AllocationHandle> local;
    local.reserve(1024);
    for (auto _ : st) {
        local.clear();
        for (int i = 0; i < 1024; ++i)
            local.push_back(pool.allocate(blockSize / 2, 16));
        for (auto& h : local)
            pool.deallocate(h);
    }
    st.counters["bytes_in_use"] = static_cast<double>(pool.stats().bytesInUse.load());
}

BENCHMARK(BM_Pool_MPMC)
    ->Args({64, 256, 0})->Args({64, 256, 1})
    ->Args({64, 1024, 0})->Args({64, 1024, 1})
    ->Threads(1)->Threads(2)->Threads(4)->Threads(8);

static void BM_GP_Mixed(benchmark::State& st)
{
    const Size threshold = static_cast<Size>(st.range(0));
    const bool useProv = static_cast<bool>(st.range(1));
    CpuBlockProvider provider;
    GeneralPurposeAllocator gp(threshold, useProv ? &provider : nullptr);
    for (auto _ : st) {
        // 模拟混合负载：小块+大块
        AllocationHandle h1 = gp.allocate(64, 16);
        AllocationHandle h2 = gp.allocate(4096, 64);
        gp.deallocate(h1);
        gp.deallocate(h2);
    }
    st.counters["bytes_in_use"] = static_cast<double>(gp.stats().bytesInUse.load());
}

BENCHMARK(BM_GP_Mixed)
    ->Args({256, 0})->Args({256, 1})
    ->Args({512, 0})->Args({512, 1})
    ->Threads(1)->Threads(2)->Threads(4)->Threads(8);

static void BM_ScopeAllocator_MPSC(benchmark::State& st)
{
    const Size cap = 8_MiB;
    auto src = MakeCpuMemorySource(cap);
    ScopeAllocator sa(src);

    const int threads = static_cast<int>(st.range(0));
    const int perThreadAllocs = 1024;

    for (auto _ : st) {
        auto scope = sa.BeginScope();
        std::vector<std::thread> ts;
        ts.reserve(threads);
        for (int t = 0; t < threads; ++t) {
            ts.emplace_back([&] {
                for (int i = 0; i < perThreadAllocs; ++i) {
                    auto h = scope.Allocate(64, 16);
                    benchmark::DoNotOptimize(h.cpuPtr);
                }
            });
        }
        for (auto& th : ts) th.join();
        auto ticket = scope.Close();
        sa.Complete(ticket);
        benchmark::ClobberMemory();
    }
    st.counters["bytes_in_use"] = static_cast<double>(sa.stats().bytesInUse.load());
}
BENCHMARK(BM_ScopeAllocator_MPSC)->Arg(1)->Arg(2)->Arg(4)->Arg(8);

// BENCHMARK_MAIN() is generated per-target; if building multiple targets, each will have its own main.
BENCHMARK_MAIN();
