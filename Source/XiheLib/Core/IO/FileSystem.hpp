/**
 * @File FileSystem.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Error.hpp"
#include <fstream>
#include <system_error>
#include <cerrno>

namespace xihe {
inline std::string ReadFileToString(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::error_code ec(errno, std::generic_category());
        XIHE_THROW("ReadFileToString: Could not open file '{}', error {}: {}", path, ec.value(), ec.message());
    }

    const std::streampos endPos = file.tellg();
    if (endPos < 0) { XIHE_THROW("ReadFileToString: tellg failed for '{}'.", path); }

    const size_t fileSize = static_cast<size_t>(endPos);
    std::string str;
    str.resize(fileSize);

    file.seekg(0, std::ios::beg);
    if (fileSize > 0) { file.read(str.data(), static_cast<std::streamsize>(fileSize)); }

    if (!file) {
        std::error_code ec(errno, std::generic_category());
        XIHE_THROW("ReadFileToString: read failed for '{}', error {}: {}", path, ec.value(), ec.message());
    }

    return str;
}
} // namespace xihe