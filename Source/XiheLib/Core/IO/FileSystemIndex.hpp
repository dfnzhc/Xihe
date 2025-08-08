/**
 * @File FileSystemIndex.hpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/9
 * @Brief This file is part of Xihe.
 */

#pragma once

#include "Core/Base/Error.hpp"
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <string_view>

namespace xihe {
namespace fs = std::filesystem;

struct FileInfo
{
    std::string path; // 绝对路径（标准化）
    bool isDirectory{false};
    uintmax_t size{0};
    fs::file_time_type lastWrite{};
    std::string extension; // 以小写、带点，如 ".hpp"；目录为空
};

struct FileNode
{
    FileInfo info;
    std::vector<std::unique_ptr<FileNode>> children; // 仅目录有子节点
};

// 文件系统索引
class XIHE_API FileSystemIndex
{
public:
    FileSystemIndex() = default;

    bool build(const std::string& rootPath);

    XIHE_NODISCARD const FileNode* root() const { return _root.get(); }
    XIHE_NODISCARD const std::string& rootPath() const { return _rootPath; }

    // 查询：支持传入绝对或相对（相对于 root）的路径
    XIHE_NODISCARD bool exists(std::string_view relOrAbs) const;

    // 按扩展名筛选（扩展名以小写、可带或不带点）
    XIHE_NODISCARD std::vector<const FileInfo*> filterByExtension(std::string_view ext) const;
    XIHE_NODISCARD std::vector<const FileInfo*> filterByExtensions(std::initializer_list<std::string_view> exts) const;

private:
    static std::string NormalizePath(const fs::path& p);
    static std::string NormalizeExtension(std::string_view ext);

    void insertNode(const fs::path& p, const fs::directory_entry& entry, std::unordered_map<std::string, FileNode*>& nodeMap);

private:
    std::string _rootPath;
    std::unique_ptr<FileNode> _root;
    std::unordered_set<std::string> _pathSet; // 绝对路径集合
    std::unordered_map<std::string, std::vector<const FileInfo*>> _extIndex; // 扩展名 -> files
};
} // namespace xihe