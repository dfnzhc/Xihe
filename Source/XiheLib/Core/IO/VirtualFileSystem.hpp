/**
 * @File VirtualFileSystem.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <filesystem>

#include "Core/Base/Defines.hpp"

namespace xihe {
class XIHE_API VirtualFileSystem
{
public:
    VirtualFileSystem() = default;

    bool mount(std::string_view alias, std::string_view physicalPath, int priority = 0);
    bool unmount(std::string_view alias, std::string_view physicalPath);
    bool unmountAll(std::string_view alias);

    XIHE_NODISCARD bool exists(std::string_view uriOrPath) const;
    XIHE_NODISCARD std::optional<std::string> resolve(std::string_view uriOrPath) const;
    // materialize：即使目标不存在，也根据别名生成物理路径（用于写入类操作）
    XIHE_NODISCARD std::optional<std::string> materialize(std::string_view uriOrPath) const;

    // 列举：pattern 使用简单通配符（支持 * 与 ?），recursive 控制递归与否
    XIHE_NODISCARD std::vector<std::string> enumerate(std::string_view uri, std::string_view pattern = "*", bool recursive = true) const;

private:
    struct Mount
    {
        std::string alias;
        std::string root; // 规范化绝对路径（generic_string）
        int priority{0};
    };

    struct UriParts
    {
        std::string alias;
        std::string path; // 不含开头 '/'
    };

    static bool parseUri(std::string_view uri, UriParts& out);
    static std::string normalizePath(const std::filesystem::path& p);
    static bool wildcardMatch(std::string_view text, std::string_view pattern);

    std::vector<Mount> _mounts;
};
} // namespace xihe