/**
 * @File MemoryTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Memory/IAllocator.hpp>
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
    auto src           = MakeCpuMemorySource(cap, alg);
    auto* base         = static_cast<std::byte*>(src->map());
    ASSERT_NE(base, nullptr);
    // 对齐检查
    EXPECT_EQ(reinterpret_cast<uintptr_t>(base) % alg, 0u);
    // 写入边界检查（首尾字节可访问）
    base[0]       = std::byte{0xAB};
    base[cap - 1] = std::byte{0xCD};
    EXPECT_EQ(base[0], std::byte{0xAB});
    EXPECT_EQ(base[cap - 1], std::byte{0xCD});
}

class TestClass
{
public:
    explicit TestClass(int value) :
        _value(value)
    {
    }

    int getValue() const
    {
        return _value;
    }

private:
    int _value;
};

class MemorySmartPtrTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(MemorySmartPtrTest, PlainAllocatorBasic)
{
    PlainAllocator<int> allocator;

    int* ptr = allocator.allocate(10);
    ASSERT_NE(ptr, nullptr);

    // 测试使用
    for (int i = 0; i < 10; ++i)
    {
        ptr[i] = i;
    }

    for (int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(ptr[i], i);
    }

    allocator.deallocate(ptr, 10);
}

TEST_F(MemorySmartPtrTest, MakeShared)
{
    auto ptr = MakeShared<TestClass>(42);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->getValue(), 42);

    EXPECT_EQ(ptr.use_count(), 1);

    auto ptr2 = ptr;
    EXPECT_EQ(ptr.use_count(), 2);
    EXPECT_EQ(ptr2.use_count(), 2);

    ptr2.reset();
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST_F(MemorySmartPtrTest, MakeUnique)
{
    // 测试基本功能
    auto ptr = MakeUnique<TestClass>(123);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->getValue(), 123);

    // 测试移动语义
    auto ptr2 = std::move(ptr);
    EXPECT_EQ(ptr, nullptr);
    ASSERT_NE(ptr2, nullptr);
    EXPECT_EQ(ptr2->getValue(), 123);
}

TEST_F(MemorySmartPtrTest, TypeAliases)
{
    // 测试 MiSharedPtr
    SharedPtr<TestClass> sharedPtr = MakeShared<TestClass>(789);
    ASSERT_NE(sharedPtr, nullptr);
    EXPECT_EQ(sharedPtr->getValue(), 789);

    // 测试 MiUniquePtr
    UniquePtr<TestClass> uniquePtr = MakeUnique<TestClass>(101112);
    ASSERT_NE(uniquePtr, nullptr);
    EXPECT_EQ(uniquePtr->getValue(), 101112);
}

TEST_F(MemorySmartPtrTest, ExceptionSafety)
{
    class ThrowingClass
    {
    public:
        explicit ThrowingClass(bool shouldThrow)
        {
            if (shouldThrow)
            {
                throw std::runtime_error("Test exception");
            }
            _value = 42;
        }

        int getValue() const
        {
            return _value;
        }

    private:
        int _value = 0;
    };

    // 正常情况
    EXPECT_NO_THROW({ auto ptr = MakeShared<ThrowingClass>(false); EXPECT_EQ(ptr->getValue(), 42); });

    // 异常情况
    EXPECT_THROW({ auto ptr = MakeShared<ThrowingClass>(true); }, std::runtime_error);
}
