/**
 * @File FileSystemTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/IO/FileSystem.hpp>

using namespace xihe;

// 可执行程序目录 Bin/<Config>/(Libs/, **.exe)
TEST(FileSystemIndex, BuildAndQuery)
{
    FileSystemIndex idx;
    ASSERT_TRUE(idx.build("."));

    // 构建后根路径应存在
    EXPECT_TRUE(idx.exists("."));

    // 查找一个已知存在的目录/文件（尽量宽松）
    EXPECT_TRUE(idx.exists("Libs"));
}

TEST(FileSystemIndex, FilterByExtension)
{
    FileSystemIndex idx;
    ASSERT_TRUE(idx.build("."));

    auto libs = idx.filterByExtension(".lib");
    // 工程中应存在若干 .hpp 文件
    EXPECT_FALSE(libs.empty());

    auto set = idx.filterByExtensions({".exe", "dll"});
    EXPECT_FALSE(set.empty());
}

TEST(VFS, MountResolveExists)
{
    VirtualFileSystem vfs;
    ASSERT_TRUE(vfs.mount("res", ".", 10));

    auto resolved = vfs.resolve("res://Source");
    ASSERT_TRUE(resolved.has_value());
    EXPECT_TRUE(vfs.exists("res://Source/XiheLib"));
}

TEST(VFS, Enumerate)
{
    VirtualFileSystem vfs;
    ASSERT_TRUE(vfs.mount("res", "."));

    auto files = vfs.enumerate("res://Source", "*.hpp", true);
    EXPECT_FALSE(files.empty());
}

TEST(FileSystemCombined, VfsIndexAndOps)
{
    FileSystem fs;
    // 将当前工程根挂载为 res://
    ASSERT_TRUE(fs.mount("res", ".", 10));

    // 构建索引
    ASSERT_TRUE(fs.buildIndex("res://"));
    EXPECT_TRUE(fs.index().exists("Libs"));

    // 写入/读取/复制/重命名/删除
    const std::string uri = "res://.tmp_test_dir/hello.txt";
    const std::string uriCopy = "res://.tmp_test_dir/hello_copy.txt";
    const std::string uriRen = "res://.tmp_test_dir/hello_renamed.txt";

    ASSERT_TRUE(fs.createDirectories("res://.tmp_test_dir"));
    ASSERT_TRUE(fs.writeAllText(uri, "abc"));
    EXPECT_EQ(fs.readAllText(uri), "abc");

    ASSERT_TRUE(fs.copyFile(uri, uriCopy, true));
    EXPECT_EQ(fs.readAllText(uriCopy), "abc");

    ASSERT_TRUE(fs.rename(uriCopy, uriRen));
    EXPECT_EQ(fs.readAllText(uriRen), "abc");

    EXPECT_TRUE(fs.remove(uri));
    EXPECT_GT(fs.removeAll("res://.tmp_test_dir"), 0u);
}