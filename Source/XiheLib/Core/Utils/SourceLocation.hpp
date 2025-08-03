/**
 * @File SourceLocation.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

namespace xihe {
struct SourceLocation
{
    const char* file = nullptr;
    int line = 0;
    const char* function = nullptr;

    constexpr SourceLocation(const char* inFile, int inLine, const char* inFunction)
        : file(inFile), line(inLine), function(inFunction) {}
};
} // namespace xihe