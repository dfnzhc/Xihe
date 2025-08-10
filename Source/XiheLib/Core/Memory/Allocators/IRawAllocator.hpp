/**
 * @File IRawAllocator.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Memory/Memory.hpp"

namespace xihe {

class IRawAllocator
{
public:
    virtual ~IRawAllocator() = default;
    virtual AllocationHandle allocate(Size size, Size alignment) = 0;
    virtual void deallocate(const AllocationHandle& h) = 0;
    virtual const AllocationStatistics& stats() const = 0;
};
} // namespace xihe
