/**
 * @File FileSystem.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Error.hpp"
#include <fstream>

namespace xihe {
inline std::string ReadFileToString(const std::string& path)
{
    std::ifstream file(path, std::ios::ate);
    if (!file.good())
        XIHE_THROW("ReadFileToString: Could not open file '{}'.", path);

    const auto fileSize = file.tellg();
    file.seekg(std::ios::beg);
    std::string str(fileSize, 0);
    file.read(str.data(), fileSize);
    return str;
}
} // namespace xihe