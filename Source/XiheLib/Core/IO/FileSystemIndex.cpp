/**
 * @File FileSystemIndex.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/9
 * @Brief This file is part of Xihe.
 */

#include "FileSystemIndex.hpp"

using namespace xihe;
namespace fs = std::filesystem;

std::string FileSystemIndex::NormalizePath(const fs::path& p)
{
    std::error_code ec;
    auto norm = fs::weakly_canonical(p, ec).generic_string();
    if (ec)
        norm = fs::absolute(p, ec).generic_string();
    return norm;
}

std::string FileSystemIndex::NormalizeExtension(std::string_view ext)
{
    std::string e;
    e.reserve(ext.size() + 1);
    if (!ext.empty() && ext[0] != '.')
        e.push_back('.');
    for (char c : ext)
        e.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    return e;
}

void FileSystemIndex::insertNode(const fs::path& p, const fs::directory_entry& entry, std::unordered_map<std::string, FileNode*>& nodeMap)
{
    const auto abs = NormalizePath(p);

    auto node = std::make_unique<FileNode>();
    node->info.path = abs;
    node->info.isDirectory = entry.is_directory();
    if (!node->info.isDirectory) {
        std::error_code ec;
        node->info.size = entry.file_size(ec);
        node->info.extension = NormalizeExtension(entry.path().extension().generic_string());
    }
    std::error_code ec;
    node->info.lastWrite = entry.last_write_time(ec);

    auto raw = node.get();
    nodeMap.emplace(abs, raw);

    // insert into path set and ext index
    _pathSet.insert(abs);
    if (!raw->info.isDirectory && !raw->info.extension.empty()) { _extIndex[raw->info.extension].push_back(&raw->info); }

    // attach to parent if exists
    auto parentPath = NormalizePath(p.parent_path());
    if (auto it = nodeMap.find(parentPath); it != nodeMap.end()) { it->second->children.emplace_back(std::move(node)); }
    else {
        // root
        _root = std::move(node);
    }
}

bool FileSystemIndex::build(const std::string& rootPath)
{
    _root.reset();
    _pathSet.clear();
    _extIndex.clear();

    std::error_code ec;
    fs::path root(rootPath);
    if (!fs::exists(root, ec))
        return false;
    _rootPath = NormalizePath(root);

    std::unordered_map<std::string, FileNode*> nodeMap;

    // ensure root node exists
    fs::directory_entry rootEntry(rootPath, ec);
    if (ec)
        return false;
    insertNode(root, rootEntry, nodeMap);

    for (auto it = fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec);
         !ec && it != fs::recursive_directory_iterator(); ++it) { insertNode(it->path(), *it, nodeMap); }

    return _root != nullptr;
}

bool FileSystemIndex::exists(std::string_view relOrAbs) const
{
    fs::path p(relOrAbs);
    if (p.is_relative())
        p = fs::path(_rootPath) / p;
    const auto abs = NormalizePath(p);
    return _pathSet.contains(abs);
}

std::vector<const FileInfo*> FileSystemIndex::filterByExtension(std::string_view ext) const
{
    std::vector<const FileInfo*> res;
    const auto key = NormalizeExtension(ext);
    if (auto it = _extIndex.find(key); it != _extIndex.end())
        res = it->second;
    return res;
}

std::vector<const FileInfo*> FileSystemIndex::filterByExtensions(std::initializer_list<std::string_view> exts) const
{
    std::vector<const FileInfo*> res;
    std::unordered_set<const FileInfo*> uniq;
    for (auto e : exts) {
        const auto key = NormalizeExtension(e);
        if (auto it = _extIndex.find(key); it != _extIndex.end()) {
            for (auto* fi : it->second)
                if (uniq.insert(fi).second)
                    res.push_back(fi);
        }
    }
    return res;
}