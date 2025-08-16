/**
 * @File Memory.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Concepts.hpp"
#include "Core/Base/Defines.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wzero-as-null-pointer-constant")
#include <mimalloc.h>
// #include <mimalloc-override.h>
XIHE_POP_WARNING

#include <cstddef>
#include <cstdint>
#include <memory>

namespace xihe {
// -----------------------------

class IRawAllocator;

/**
 * AllocationHandle：一次分配的句柄（CPU 侧）。
 *
 * - cpuPtr：CPU 可访问指针（不可用则为 nullptr）。
 * - size/alignment/offset：分配的元信息。
 *
 * 使用示例：
 * auto h = allocator.allocate(256, 16);
 * std::memcpy(h.GetCpuPointer(), src, h.size);
 * allocator.deallocate(h);
 */
struct AllocationHandle
{
    void* cpuPtr{nullptr};
    Size size{0};
    Size alignment{0};
    Size offset{0};

    IRawAllocator* allocatorId{nullptr};

    XIHE_NODISCARD void* GetCpuPointer() const { return cpuPtr; }
    XIHE_NODISCARD explicit operator bool() const { return cpuPtr != nullptr; }
};

/**
 * AllocationStatistics：分配器统计信息（原子）。
 */
struct AllocationStatistics
{
    std::atomic<Size> numAllocations{0};
    std::atomic<Size> numFrees{0};
    std::atomic<Size> bytesInUse{0};
    std::atomic<Size> peakBytes{0};

    void onAllocate(Size sz)
    {
        numAllocations.fetch_add(1, std::memory_order_relaxed);
        auto cur = bytesInUse.fetch_add(sz, std::memory_order_relaxed) + sz;
        Size prev = peakBytes.load(std::memory_order_relaxed);
        while (cur > prev && !peakBytes.compare_exchange_weak(prev, cur)) {}
    }

    void onFree(Size sz)
    {
        numFrees.fetch_add(1, std::memory_order_relaxed);
        bytesInUse.fetch_sub(sz, std::memory_order_relaxed);
    }
};

// -----------------------------

enum class MemorySourceKind : uint32_t { CPU = 0, GPUOnly, Upload, Readback };

class IMemorySource
{
public:
    virtual ~IMemorySource() = default;

    virtual size_t size() const = 0;      // 总字节数
    virtual size_t alignment() const = 0; // 最低物理对齐约束
    virtual MemorySourceKind kind() const = 0;

    // CPU: 返回可访问指针；GPUOnly: 可能返回 nullptr；Upload/Readback: 返回映射指针
    virtual void* map() = 0;
    virtual void unmap() = 0;

    // CPU: 返回 base 指针；GPU: 返回底层句柄（例如 VkDeviceMemory/ID3D12Heap*）
    virtual void* nativeHandle() const = 0;
};

using MemorySourcePtr = std::shared_ptr<IMemorySource>;

// -----------------------------

class CpuMemorySource : public IMemorySource
{
public:
    explicit CpuMemorySource(Size capacityBytes, Size align = alignof(std::max_align_t))
        : _size(capacityBytes), _alignment(align) { _base = mi_malloc_aligned(_size, _alignment); }

    ~CpuMemorySource() override
    {
        if (_base) {
            mi_free(_base);
            _base = nullptr;
        }
    }

    XIHE_NODISCARD Size size() const override { return _size; }
    XIHE_NODISCARD Size alignment() const override { return _alignment; }
    XIHE_NODISCARD MemorySourceKind kind() const override { return MemorySourceKind::CPU; }

    void* map() override { return _base; }
    void unmap() override {}
    void* nativeHandle() const override { return _base; }

private:
    void* _base{nullptr};
    Size _size{0};
    Size _alignment{alignof(std::max_align_t)};
};

inline std::shared_ptr<CpuMemorySource> MakeCpuMemorySource(Size bytes, Size align = alignof(std::max_align_t))
{
    return std::make_shared<CpuMemorySource>(bytes, align);
}

// -----------------------------

class IBlockProvider
{
public:
    virtual ~IBlockProvider() = default;
    virtual void* allocateBlock(size_t bytes, size_t alignment) = 0;
    virtual void freeBlock(void* base, size_t bytes) = 0;
};

class CpuBlockProvider : public IBlockProvider
{
public:
    void* allocateBlock(size_t bytes, size_t alignment) override { return mi_malloc_aligned(bytes, alignment); }

    void freeBlock(void* base, size_t) override { mi_free(base); }
};

// -----------------------------
// 用于表示内存大小的自定义字面量类型
// KB/MB/GB 以 1000 为乘数
// KiB/MiB/GiB 以 1024 为乘数
// 基础单位是 B(bytes)

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-identifier")
constexpr Size operator"" _B(unsigned long long v) { return As<Size>(v); }
constexpr Size operator"" _KB(unsigned long long v) { return As<Size>(v) * 1000ull; }
constexpr Size operator"" _MB(unsigned long long v) { return As<Size>(v) * 1000ull * 1000ull; }
constexpr Size operator"" _GB(unsigned long long v) { return As<Size>(v) * 1000ull * 1000ull * 1000ull; }

constexpr Size operator"" _KiB(unsigned long long v) { return As<Size>(v) * 1024ull; }
constexpr Size operator"" _MiB(unsigned long long v) { return As<Size>(v) * 1024ull * 1024ull; }
constexpr Size operator"" _GiB(unsigned long long v) { return As<Size>(v) * 1024ull * 1024ull * 1024ull; }
XIHE_POP_WARNING


// -----------------------------

#define XIHE_SAFE_DELETE_PTR(ptr) do { if (ptr){ delete ptr; ptr = nullptr; } } while(0)
#define XIHE_SAFE_RESET_PTR(ptr) do { if (ptr){ ptr.reset(); ptr = nullptr; } } while(0)
} // namespace xihe
