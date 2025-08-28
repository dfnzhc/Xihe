/**
 * @File MemoryBench.cpp
 */

#include <benchmark/benchmark.h>
#include <Core/Memory/IAllocator.hpp>
#include <Core/Memory/Memory.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <random>

using namespace xihe;

// 测试用的简单类
class TestObject
{
public:
    TestObject(int value = 0) :
        data(value)
    {
        // 模拟一些初始化工作
        for (int i = 0; i < 10; ++i)
        {
            buffer[i] = value + i;
        }
    }

    int getValue() const
    {
        return data;
    }

private:
    int data;
    int buffer[10]; // 增加对象大小
};

// 基准测试：单次分配性能对比
static void BM_SingleAllocation_StdMakeShared(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto ptr = std::make_shared<TestObject>(42);
        benchmark::DoNotOptimize(ptr);
    }
}

BENCHMARK(BM_SingleAllocation_StdMakeShared);

static void BM_SingleAllocation_XiheMakeShared(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto ptr = MakeShared<TestObject>(42);
        benchmark::DoNotOptimize(ptr);
    }
}

BENCHMARK(BM_SingleAllocation_XiheMakeShared);

// 基准测试：批量分配性能对比
static void BM_BatchAllocation_StdMakeShared(benchmark::State& state)
{
    const int batch_size = state.range(0);
    for (auto _ : state)
    {
        std::vector<std::shared_ptr<TestObject>> ptrs;
        ptrs.reserve(batch_size);
        for (int i = 0; i < batch_size; ++i)
        {
            ptrs.push_back(std::make_shared<TestObject>(i));
        }
        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_BatchAllocation_StdMakeShared)->Range(100, 10000);

static void BM_BatchAllocation_XiheMakeShared(benchmark::State& state)
{
    const int batch_size = state.range(0);
    for (auto _ : state)
    {
        std::vector<SharedPtr<TestObject>> ptrs;
        ptrs.reserve(batch_size);
        for (int i = 0; i < batch_size; ++i)
        {
            ptrs.push_back(MakeShared<TestObject>(i));
        }
        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_BatchAllocation_XiheMakeShared)->Range(100, 10000);

// 基准测试：混合大小对象分配
static void BM_XihexedSizeAllocation_Std(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    for (auto _ : state)
    {
        std::vector<std::shared_ptr<TestObject>> ptrs;
        for (int i = 0; i < 100; ++i)
        {
            ptrs.push_back(std::make_shared<TestObject>(dis(gen)));
        }
        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_XihexedSizeAllocation_Std);

static void BM_XihexedSizeAllocation_Xihe(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    for (auto _ : state)
    {
        std::vector<SharedPtr<TestObject>> ptrs;
        for (int i = 0; i < 100; ++i)
        {
            ptrs.push_back(MakeShared<TestObject>(dis(gen)));
        }
        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_XihexedSizeAllocation_Xihe);

// 基准测试：频繁分配释放场景
static void BM_FrequentAllocDealloc_Std(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i)
        {
            auto ptr = std::make_shared<TestObject>(i);
            // 立即释放，模拟频繁分配释放
        }
    }
}

BENCHMARK(BM_FrequentAllocDealloc_Std);

static void BM_FrequentAllocDealloc_Xihe(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i)
        {
            auto ptr = MakeShared<TestObject>(i);
            // 立即释放，模拟频繁分配释放
        }
    }
}

BENCHMARK(BM_FrequentAllocDealloc_Xihe);

// 基准测试：并发分配场景
static void BM_ConcurrentAllocation_Std(benchmark::State& state)
{
    const int num_threads = state.range(0);

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        std::vector<std::vector<std::shared_ptr<TestObject>>> results(num_threads);

        for (int t = 0; t < num_threads; ++t)
        {
            threads.emplace_back([&results, t]()
            {
                for (int i = 0; i < 100; ++i)
                {
                    results[t].push_back(std::make_shared<TestObject>(i));
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        benchmark::DoNotOptimize(results);
    }
}

BENCHMARK(BM_ConcurrentAllocation_Std)->Range(2, 8);

static void BM_ConcurrentAllocation_Xihe(benchmark::State& state)
{
    const int num_threads = state.range(0);

    for (auto _ : state)
    {
        std::vector<std::thread> threads;
        std::vector<std::vector<SharedPtr<TestObject>>> results(num_threads);

        for (int t = 0; t < num_threads; ++t)
        {
            threads.emplace_back([&results, t]()
            {
                for (int i = 0; i < 100; ++i)
                {
                    results[t].push_back(MakeShared<TestObject>(i));
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        benchmark::DoNotOptimize(results);
    }
}

BENCHMARK(BM_ConcurrentAllocation_Xihe)->Range(2, 8);

// 基准测试：内存碎片化场景
static void BM_FragmentationScenario_Std(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<std::shared_ptr<TestObject>> ptrs;

        // 分配大量对象
        for (int i = 0; i < 1000; ++i)
        {
            ptrs.push_back(std::make_shared<TestObject>(i));
        }

        // 随机释放一半对象，造成内存碎片
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(ptrs.begin(), ptrs.end(), gen);
        ptrs.resize(500);

        // 再次分配对象
        for (int i = 0; i < 500; ++i)
        {
            ptrs.push_back(std::make_shared<TestObject>(i + 1000));
        }

        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_FragmentationScenario_Std);

static void BM_FragmentationScenario_Xihe(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<SharedPtr<TestObject>> ptrs;

        // 分配大量对象
        for (int i = 0; i < 1000; ++i)
        {
            ptrs.push_back(MakeShared<TestObject>(i));
        }

        // 随机释放一半对象，造成内存碎片
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(ptrs.begin(), ptrs.end(), gen);
        ptrs.resize(500);

        // 再次分配对象
        for (int i = 0; i < 500; ++i)
        {
            ptrs.push_back(MakeShared<TestObject>(i + 1000));
        }

        benchmark::DoNotOptimize(ptrs);
    }
}

BENCHMARK(BM_FragmentationScenario_Xihe);

BENCHMARK_MAIN();
