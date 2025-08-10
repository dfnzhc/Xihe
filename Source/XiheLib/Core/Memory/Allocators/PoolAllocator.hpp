/**
 * @File PoolAllocator.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <mutex>
#include <vector>
#include "Core/Memory/Allocators/IRawAllocator.hpp"

namespace xihe {
/**
 * 只进行固定大小分配的池分配器：面向小对象的高效重用
 *
 * 特性:
 * - 固定块大小，O(1) 分配/释放；内部使用 free list
 * - 线程安全；可扩展线程局部缓存
 * - 适合频繁创建/销毁的小对象，如组件、句柄、节点
 * - 支持传入自定义的内存提供者 IBlockProvider 
 */
class PoolAllocator : public IRawAllocator
{
public:
    explicit PoolAllocator(Size blockSize, Size blocksPerChunk = 1024, IBlockProvider* provider = nullptr)
        : _blockSize(blockSize < sizeof(void*) ? sizeof(void*) : blockSize), _blocksPerChunk(blocksPerChunk), _provider(provider) {}

    ~PoolAllocator() override
    {
        const Size bytes = _blockSize * _blocksPerChunk;
        for (void* p : _chunks) {
            if (p != nullptr) {
                if (_provider) { _provider->freeBlock(p, bytes); }
                else { mi_free(p); }
            }
        }
        _chunks.clear();
        _freeList = nullptr;
    }

    AllocationHandle allocate(Size size, Size alignment) override
    {
        (void)alignment;
        if (size > _blockSize)
            return {};
        std::lock_guard lk(_mtx);
        if (!_freeList)
            expand();
        void* p = _freeList;
        _freeList = *reinterpret_cast<void**>(_freeList);
        _stats.onAllocate(_blockSize);
        return {p, _blockSize, alignof(std::max_align_t), 0, this};
    }

    void deallocate(const AllocationHandle& h) override
    {
        std::lock_guard lk(_mtx);
        *reinterpret_cast<void**>(h.cpuPtr) = _freeList;
        _freeList = h.cpuPtr;
        _stats.onFree(_blockSize);
    }

    const AllocationStatistics& stats() const override { return _stats; }

private:
    XIHE_PUSH_WARNING
    XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")

    void expand()
    {
        const Size bytes = _blockSize * _blocksPerChunk;
        std::byte* base = nullptr;
        if (_provider) { base = static_cast<std::byte*>(_provider->allocateBlock(bytes, alignof(std::max_align_t))); }
        if (!base) { base = static_cast<std::byte*>(mi_malloc_aligned(bytes, alignof(std::max_align_t))); }
        _chunks.push_back(base);
        for (Size i = 0; i < _blocksPerChunk; ++i) {
            void* node = base + i * _blockSize;
            *reinterpret_cast<void**>(node) = _freeList;
            _freeList = node;
        }
    }

    XIHE_POP_WARNING

    Size _blockSize;
    Size _blocksPerChunk;
    std::vector<void*> _chunks;
    void* _freeList{nullptr};
    mutable std::mutex _mtx;
    AllocationStatistics _stats;
    IBlockProvider* _provider{nullptr};
};
} // namespace xihe
