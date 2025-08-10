/**
 * @File ScopeAllocator.hpp
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
#include <unordered_set>
#include <vector>

#include "Core/Memory/Memory.hpp"
#include "Core/Utils/Ring.hpp"

namespace xihe {
class ScopeAllocator
{
public:
    struct RingTicket
    {
        u64 endCounter{0};
        Size index{As<Size>(-1)};
        bool isValid() const { return index != As<Size>(-1); }
    };

    explicit ScopeAllocator(MemorySourcePtr source)
        : _source(std::move(source))
          , _ring(_source ? As<Size>(_source->size()) : 0) { if (_source) { _base = static_cast<std::byte*>(_source->map()); } }

    // Scope 聚合器
    class Scope
    {
    public:
        explicit Scope(ScopeAllocator& owner)
            : _owner(owner) {}

        AllocationHandle Allocate(Size size, Size alignment)
        {
            Ring::ReserveResult r{};
            if (!_owner._ring.tryReserve(size, alignment, r))
                return {};
            _owner._stats.onAllocate(r.reservedBytes);
            _endCounter = r.endCounter;
            _hasAllocation = true;
            
            XIHE_PUSH_WARNING
            XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
            return {_owner._base + r.finalOffset, size, alignment, r.finalOffset, nullptr};
            XIHE_POP_WARNING
        }

        RingTicket Close()
        {
            if (_closed)
                return _ticket;
            _closed = true;
            if (!_hasAllocation) {
                _ticket = RingTicket{};
                return _ticket;
            }
            _ticket = _owner.registerTicket(_endCounter);
            return _ticket;
        }

    private:
        ScopeAllocator& _owner;
        u64 _endCounter{0};
        bool _hasAllocation{false};
        bool _closed{false};
        RingTicket _ticket{};
    };

    Scope BeginScope() { return Scope(*this); }

    void Complete(RingTicket ticket)
    {
        if (!ticket.isValid()) return;
        std::unique_lock<std::mutex> lk(_ticketMutex);
        _completed.insert(ticket.index);

        while (!_tickets.empty() && _completed.contains(_tickets.front().index)) {
            u64 newTail = _tickets.front().endCounter;
            u64 oldTail = _ring.tailCounter();
            _ring.setTail(newTail);
            _stats.onFree(As<Size>(newTail - oldTail));

            _completed.erase(_tickets.front().index);
            _tickets.pop_front();
        }
    }

    // 调试：在无挂起 Ticket 时清空占用
    void ForceDrainUnsafe()
    {
        u64 head = _ring.headCounter();
        u64 tail = _ring.tailCounter();
        if (head > tail) {
            _ring.setTail(head);
            _stats.bytesInUse.store(0, std::memory_order_relaxed);
        }
    }

    const AllocationStatistics& stats() const { return _stats; }

private:
    struct TicketNode { u64 endCounter{0}; Size index{0}; };

    RingTicket registerTicket(u64 endCounter)
    {
        std::lock_guard<std::mutex> lk(_ticketMutex);
        const Size idx = _nextTicketIndex++;
        _tickets.push_back(TicketNode{endCounter, idx});
        return RingTicket{endCounter, idx};
    }

private:
    MemorySourcePtr _source{nullptr};
    std::byte* _base{nullptr};
    Ring _ring{0};

    std::mutex _ticketMutex;
    std::deque<TicketNode> _tickets;
    std::unordered_set<Size> _completed; // 已完成 ticket 索引集合
    Size _nextTicketIndex{0};

    AllocationStatistics _stats;
};
} // namespace xihe
