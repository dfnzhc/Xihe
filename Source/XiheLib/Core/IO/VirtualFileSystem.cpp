/**
 * @File VirtualFileSystem.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include "VirtualFileSystem.hpp"

using namespace xihe;
namespace fs = std::filesystem;

std::string VirtualFileSystem::normalizePath(const fs::path& p)
{
    std::error_code ec;
    auto norm = fs::weakly_canonical(p, ec).generic_string();
    if (ec)
        norm = fs::absolute(p, ec).generic_string();
    return norm;
}

bool VirtualFileSystem::parseUri(std::string_view uri, UriParts& out)
{
    // 格式： alias://path/to/file
    auto pos = uri.find("://");
    if (pos == std::string_view::npos)
        return false;
    out.alias = std::string(uri.substr(0, pos));
    auto rest = uri.substr(pos + 3);
    if (!rest.empty() && rest.front() == '/')
        rest.remove_prefix(1);
    out.path = std::string(rest);
    return !out.alias.empty();
}

bool VirtualFileSystem::wildcardMatch(std::string_view text, std::string_view pattern)
{
    // 简单通配符匹配：* 任意串；? 单字符
    size_t ti = 0, pi = 0, star = std::string::npos, match = 0;
    while (ti < text.size()) {
        if (pi < pattern.size() && (pattern[pi] == '?' || pattern[pi] == text[ti])) {
            ++ti;
            ++pi;
        }
        else if (pi < pattern.size() && pattern[pi] == '*') {
            star = pi++;
            match = ti;
        }
        else if (star != std::string::npos) {
            pi = star + 1;
            ti = ++match;
        }
        else { return false; }
    }
    while (pi < pattern.size() && pattern[pi] == '*')
        ++pi;
    return pi == pattern.size();
}

bool VirtualFileSystem::mount(std::string_view alias, std::string_view physicalPath, int priority)
{
    std::error_code ec;
    fs::path p(physicalPath);
    if (!fs::exists(p, ec))
        return false;
    Mount m{std::string(alias), normalizePath(p), priority};
    _mounts.push_back(std::move(m));
    std::sort(_mounts.begin(),
              _mounts.end(),
              [](const Mount& a, const Mount& b) {
                  if (a.alias != b.alias)
                      return a.alias < b.alias;
                  return a.priority > b.priority; // 优先级高的在前
              });
    return true;
}

bool VirtualFileSystem::unmount(std::string_view alias, std::string_view physicalPath)
{
    std::string root = normalizePath(fs::path(std::string(physicalPath)));
    auto before = _mounts.size();
    _mounts.erase(std::remove_if(_mounts.begin(), _mounts.end(), [&](const Mount& m) { return m.alias == alias && m.root == root; }), _mounts.end());
    return _mounts.size() != before;
}

bool VirtualFileSystem::unmountAll(std::string_view alias)
{
    auto before = _mounts.size();
    _mounts.erase(std::remove_if(_mounts.begin(), _mounts.end(), [&](const Mount& m) { return m.alias == alias; }), _mounts.end());
    return _mounts.size() != before;
}

std::optional<std::string> VirtualFileSystem::resolve(std::string_view uriOrPath) const
{
    UriParts parts;
    if (!parseUri(uriOrPath, parts)) {
        // 非 URI，当作物理路径
        std::error_code ec;
        if (fs::exists(uriOrPath, ec))
            return normalizePath(uriOrPath.data());
        return std::nullopt;
    }

    for (const auto& m : _mounts) {
        if (m.alias != parts.alias)
            continue;
        fs::path p = fs::path(m.root) / parts.path;
        std::error_code ec;
        if (fs::exists(p, ec))
            return normalizePath(p);
    }
    return std::nullopt;
}

bool VirtualFileSystem::exists(std::string_view uriOrPath) const { return resolve(uriOrPath).has_value(); }

std::optional<std::string> VirtualFileSystem::materialize(std::string_view uriOrPath) const
{
    UriParts parts;
    if (!parseUri(uriOrPath, parts)) { return normalizePath(uriOrPath.data()); }

    for (const auto& m : _mounts) {
        if (m.alias != parts.alias)
            continue;
        fs::path p = fs::path(m.root) / parts.path;
        return normalizePath(p);
    }
    return std::nullopt;
}

std::vector<std::string> VirtualFileSystem::enumerate(std::string_view uri, std::string_view pattern, bool recursive) const
{
    std::vector<std::string> result;
    UriParts parts;
    if (!parseUri(uri, parts))
        return result;

    for (const auto& m : _mounts) {
        if (m.alias != parts.alias)
            continue;
        fs::path base = fs::path(m.root) / parts.path;
        std::error_code ec;
        if (!fs::exists(base, ec))
            continue;

        if (recursive) {
            for (auto it = fs::recursive_directory_iterator(base, fs::directory_options::skip_permission_denied, ec);
                 !ec && it != fs::recursive_directory_iterator(); ++it) {
                if (it->is_directory())
                    continue;
                auto rel = fs::relative(it->path(), base, ec).generic_string();
                if (wildcardMatch(rel, pattern))
                    result.push_back(normalizePath(it->path()));
            }
        }
        else {
            for (auto it = fs::directory_iterator(base, ec); !ec && it != fs::directory_iterator(); ++it) {
                if (it->is_directory())
                    continue;
                auto name = it->path().filename().generic_string();
                if (wildcardMatch(name, pattern))
                    result.push_back(normalizePath(it->path()));
            }
        }
    }
    return result;
}