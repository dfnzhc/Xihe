/**
 * @File GeneralPurposeAllocator.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Memory/Allocators/PoolAllocator.hpp"
#include "Core/Memory/Memory.hpp"

namespace xihe {
/**
 * 通用分配器：小分配走池，大分配走系统（mimalloc）
 *
 * 特性:
 * - 小尺寸（默认 <= 512B）分配委托给内置 PoolAllocator
 * - 大尺寸使用 mimalloc 对齐分配/释放
 * - 兼顾通用性与性能，适合集成到 MemoryManager 的默认路径
 * - 支持传入自定义的内存提供者 IBlockProvider 
 */
class GeneralPurposeAllocator : public IRawAllocator
{
public:
    explicit GeneralPurposeAllocator(Size smallBlockSize = 512_B, IBlockProvider* provider = nullptr)
        : _smallThreshold(smallBlockSize), _pool(smallBlockSize), _provider(provider) {}

    AllocationHandle allocate(Size size, Size alignment) override
    {
        if (size <= _smallThreshold) { return _pool.allocate(size, alignment); }
        void* p = nullptr;
        if (_provider) { p = _provider->allocateBlock(size, alignment); }
        if (!p) { p = mi_malloc_aligned(size, alignment); }
        _stats.onAllocate(size);
        return {p, size, alignment, 0, this};
    }

    void deallocate(const AllocationHandle& h) override
    {
        if (h.size <= _smallThreshold) {
            _pool.deallocate(h);
            return;
        }
        _stats.onFree(h.size);
        if (_provider) { _provider->freeBlock(h.cpuPtr, h.size); }
        else { mi_free(h.cpuPtr); }
    }

    const AllocationStatistics& stats() const override { return _stats; }

private:
    Size _smallThreshold{512_B};
    PoolAllocator _pool;
    AllocationStatistics _stats;
    IBlockProvider* _provider{nullptr};
};
} // namespace xihe
