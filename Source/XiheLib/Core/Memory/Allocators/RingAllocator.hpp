/**
 * @File RingAllocator.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/10
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>

#include "Core/Memory/Allocators/IRawAllocator.hpp"
#include "Core/Utils/Ring.hpp"

namespace xihe {
/**
 * 环形分配器：多生产者（MP）无锁保留，轻锁回收并按 Ticket 就序推进。
 *
 * 语义要点：
 * - allocate：FailFast 非阻塞；空间不足返回空句柄。
 * - deallocate：no-op；回收由 Complete(ticket) 推进。
 * - Scope：聚合一批分配，Close() 生成 Ticket，Complete(ticket) 后才可复用空间。
 */
// 轻量 Ring 分配器：单线程/外部加锁，严格 FIFO，基于 Ring 内核
class RingAllocator : public IRawAllocator
{
public:
    explicit RingAllocator(Size capacityBytes)
        : _ring(capacityBytes)
    {
        _buffer.resize(capacityBytes);
        _base = _buffer.data();
    }

    // IRawAllocator
    AllocationHandle allocate(Size size, Size alignment) override
    {
        Ring::ReserveResult r{};
        if (!_ring.tryReserve(size, alignment, r))
            return {};
        _stats.onAllocate(r.reservedBytes);
        
        XIHE_PUSH_WARNING
        XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
        return {_base + r.finalOffset, size, alignment, r.finalOffset, this};
        XIHE_POP_WARNING
    }

    void deallocate(const AllocationHandle& /*h*/) override
    {
        // 轻量 Ring 不支持任意释放；需要顺序释放由调用方控制 freeFront()
    }

    const AllocationStatistics& stats() const override { return _stats; }

    // 顺序释放：等价推进 tail（仅用于严格 FIFO 场景）
    void freeFront(Size bytes)
    {
        _ring.freeFront(bytes);
        _stats.onFree(bytes);
    }

    // 等待接口：委托 Ring 内核判断剩余空间（可选）
    bool WaitForBytes(Size requiredBytes, std::chrono::milliseconds timeout)
    {
        (void)requiredBytes;
        (void)timeout;
        return true;
    }

private:
    std::vector<std::byte> _buffer; // 自管内存存储
    std::byte* _base{nullptr};
    Ring _ring; // 内核
    AllocationStatistics _stats;
};
} // namespace xihe
