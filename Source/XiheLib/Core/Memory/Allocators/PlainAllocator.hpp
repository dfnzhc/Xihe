/**
 * @File PlainAllocator.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */
 
#pragma once

#include <mutex>
#include <vector>
#include "Core/Memory/Allocators/IRawAllocator.hpp"

namespace xihe {

class PlainAllocator : public IRawAllocator
{
public:
    PlainAllocator() = default;
    ~PlainAllocator() override = default;

    AllocationHandle allocate(Size size, Size alignment) override
    {
        void* p = mi_malloc_aligned(size, alignment);
        _stats.onAllocate(size);
        return {p, size, alignment, 0, this};
    }

    void deallocate(const AllocationHandle& h) override
    {
        std::lock_guard lk(_mtx);
        _stats.onFree(h.size);
        mi_free_aligned(h.cpuPtr, h.alignment);
    }

    const AllocationStatistics& stats() const override { return _stats; }

private:
    mutable std::mutex _mtx;
    AllocationStatistics _stats;
};
} // namespace xihe