/**
 * @File FileSystem.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include "FileSystem.hpp"

using namespace xihe;
namespace fs = std::filesystem;

namespace {
inline std::string ReadFileToString(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::error_code ec(errno, std::generic_category());
        XIHE_THROW("ReadFileToString: Could not open file '{}', error {}: {}", path, ec.value(), ec.message());
    }

    const std::streampos endPos = file.tellg();
    if (endPos < 0) { XIHE_THROW("ReadFileToString: tellg failed for '{}'.", path); }

    const auto fileSize = static_cast<Size>(endPos);
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
} // namespace

bool FileSystem::buildIndex(std::string_view rootUri)
{
    auto p = _vfs.resolve(rootUri);
    if (!p)
        return false;
    return _index.build(*p);
}

// 文件操作（写）：create/delete/copy/rename
bool FileSystem::createDirectories(std::string_view uri) const
{
    auto p = _vfs.materialize(uri);
    if (!p)
        return false;
    std::error_code ec;
    return std::filesystem::create_directories(*p, ec) || !ec;
}

bool FileSystem::remove(std::string_view uri) const
{
    auto p = _vfs.materialize(uri);
    if (!p)
        return false;
    std::error_code ec;
    return std::filesystem::remove(*p, ec);
}

uintmax_t FileSystem::removeAll(std::string_view uri) const
{
    auto p = _vfs.materialize(uri);
    if (!p)
        return 0;
    std::error_code ec;
    return std::filesystem::remove_all(*p, ec);
}

bool FileSystem::copyFile(std::string_view fromUri, std::string_view toUri, bool overwrite) const
{
    auto src = _vfs.resolve(fromUri);
    if (!src)
        return false;
    auto dst = _vfs.materialize(toUri);
    if (!dst)
        return false;
    std::error_code ec;
    auto opts = overwrite ? (std::filesystem::copy_options::overwrite_existing) : std::filesystem::copy_options{};
    std::filesystem::create_directories(std::filesystem::path(*dst).parent_path(), ec);
    std::filesystem::copy_file(*src, *dst, opts, ec);
    return !ec;
}

bool FileSystem::rename(std::string_view fromUri, std::string_view toUri) const
{
    auto src = _vfs.materialize(fromUri);
    auto dst = _vfs.materialize(toUri);
    if (!src || !dst)
        return false;
    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(*dst).parent_path(), ec);
    std::filesystem::rename(*src, *dst, ec);
    return !ec;
}

// 读写数据
std::string FileSystem::readAllText(std::string_view uri) const
{
    auto p = _vfs.resolve(uri);
    if (!p)
        XIHE_THROW("FileSystem.readAllText: resolve failed for '{}'.", std::string(uri));
    return ReadFileToString(*p);
}

bool FileSystem::writeAllText(std::string_view uri, std::string_view text) const
{
    auto p = _vfs.materialize(uri);
    if (!p)
        return false;
    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(*p).parent_path(), ec);
    std::ofstream ofs(*p, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open())
        return false;
    ofs.write(text.data(), static_cast<std::streamsize>(text.size()));
    return ofs.good();
}