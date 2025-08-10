/**
 * @File Ring.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/10
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "Core/Base/Defines.hpp"
#include "Core/Base/Concepts.hpp"
#include "Core/Math/Common.hpp"

namespace xihe {
/**
 * Ring: 通用环形缓冲内核（仅管理 head/tail 计数与对齐/绕回计算）。
 * - 使用绝对计数（u64）表示 head/tail，物理偏移通过取模 capacity 得到。
 * - 提供 tryReserve（原子 CAS）与 setTail（就序推进）原语；
 * - 提供 freeFront 便于严格 FIFO 场景顺序回收；
 * - 不持有底层内存，仅返回 offset 与 reserved 大小，外层据此计算指针与统计。
 */
class Ring
{
public:
    struct ReserveResult
    {
        bool ok{false};
        Size finalOffset{0};   // 真实可用的起始物理偏移（已跳过 padding）
        Size reservedBytes{0}; // 实际保留的字节（size + padding）
        Size paddingBytes{0};  // 跨尾时插入的填充
        u64 endCounter{0};     // 预留后的新 head 绝对计数
    };

    explicit Ring(Size capacityBytes)
        : _capacity(capacityBytes) {}

    XIHE_NODISCARD Size capacity() const { return _capacity; }

    XIHE_NODISCARD Size bytesInUse() const
    {
        u64 head = _head.load(std::memory_order_acquire);
        u64 tail = _tail.load(std::memory_order_acquire);
        return As<Size>(head - tail);
    }

    XIHE_NODISCARD u64 headCounter() const { return _head.load(std::memory_order_acquire); }
    XIHE_NODISCARD u64 tailCounter() const { return _tail.load(std::memory_order_acquire); }

    /**
     * 原子预留一段空间（含对齐与跨尾填充）。
     * - size: 逻辑请求字节；alignment: 2 的幂。
     * - 成功返回 true，并填充 ReserveResult；失败（空间不足）返回 false。
     */
    bool tryReserve(Size size, Size alignment, ReserveResult& out)
    {
        if (size == 0)
            return false;
        if (!IsPowerOfTwo(alignment))
            return false;
        if (size > _capacity)
            return false;

        for (;;) {
            u64 headSnapshot = _head.load(std::memory_order_acquire);
            u64 tailSnapshot = _tail.load(std::memory_order_acquire);

            Size used = As<Size>(headSnapshot - tailSnapshot);
            Size start = As<Size>(headSnapshot % _capacity);

            Size alignedStart = (start + (alignment - 1)) & ~(alignment - 1);
            Size padding = 0;
            if (alignedStart + size <= _capacity) {
                padding = alignedStart - start; // 可能为 0
            }
            else {
                padding = _capacity - start; // 吃掉尾部，真实分配从 0 开始
                alignedStart = 0;
            }

            Size reserve = padding + size;
            Size available = _capacity - used;
            if (reserve > available) {
                return false; // 空间不足：FailFast
            }

            u64 newHead = headSnapshot + As<u64>(reserve);
            if (_head.compare_exchange_weak(headSnapshot, newHead, std::memory_order_acq_rel,
                                            std::memory_order_acquire)) {
                out.ok = true;
                out.finalOffset = As<Size>((newHead - reserve + padding) % _capacity);
                out.reservedBytes = reserve;
                out.paddingBytes = padding;
                out.endCounter = newHead;
                return true;
            }
            // CAS 失败：重试
        }
    }

    /**
     * 就序推进 tail 到 newTail（绝对计数）。
     * - 仅当上游保证就序完成时调用；否则可能覆盖尚未完成的数据。
     */
    void setTail(u64 newTail) { _tail.store(newTail, std::memory_order_release); }

    /**
     * 顺序回收 convenience：等价 setTail(tail + bytes)。
     * - 仅用于严格 FIFO 场景。
     */
    void freeFront(Size bytes)
    {
        u64 tail = _tail.load(std::memory_order_acquire);
        _tail.store(tail + As<u64>(bytes), std::memory_order_release);
    }

private:
    Size _capacity{0};
    std::atomic<u64> _head{0};
    std::atomic<u64> _tail{0};
};
} // namespace xihe
