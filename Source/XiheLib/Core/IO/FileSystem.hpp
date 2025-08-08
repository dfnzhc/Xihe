/**
 * @File FileSystem.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Error.hpp"
#include <fstream>
#include <filesystem>
#include <vector>
#include <string_view>

#include "Core/IO/FileSystemIndex.hpp"
#include "Core/IO/VirtualFileSystem.hpp"

namespace xihe {
class XIHE_API FileSystem
{
public:
    // 挂载 & 配置
    bool mount(std::string_view alias, std::string_view physical, int priority = 0) { return _vfs.mount(alias, physical, priority); }
    bool unmount(std::string_view alias, std::string_view physical) { return _vfs.unmount(alias, physical); }
    bool unmountAll(std::string_view alias) { return _vfs.unmountAll(alias); }

    // 路径解析
    XIHE_NODISCARD std::optional<std::string> resolve(std::string_view uriOrPath) const { return _vfs.resolve(uriOrPath); }
    XIHE_NODISCARD std::optional<std::string> materialize(std::string_view uriOrPath) const { return _vfs.materialize(uriOrPath); }
    XIHE_NODISCARD bool exists(std::string_view uriOrPath) const { return _vfs.exists(uriOrPath); }

    // 索引：对某个根（常为 res://）构建索引
    bool buildIndex(std::string_view rootUri);

    XIHE_NODISCARD const FileSystemIndex& index() const { return _index; }

    // 文件操作（写）：create/delete/copy/rename
    bool createDirectories(std::string_view uri) const;

    bool remove(std::string_view uri) const;
    uintmax_t removeAll(std::string_view uri) const;

    bool copyFile(std::string_view fromUri, std::string_view toUri, bool overwrite = true) const;
    bool rename(std::string_view fromUri, std::string_view toUri) const;

    // 读写数据
    XIHE_NODISCARD std::string readAllText(std::string_view uri) const;
    bool writeAllText(std::string_view uri, std::string_view text) const;

private:
    VirtualFileSystem _vfs;
    FileSystemIndex _index;
};
} // namespace xihe