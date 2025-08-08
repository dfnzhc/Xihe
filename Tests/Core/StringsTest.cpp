/**
 * @File StringsTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/8
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>

#include <Core/Utils/Strings.hpp>

using namespace xihe;

TEST(Strings, Trim)
{
    std::string s = "  hello  ";
    EXPECT_EQ(Trim(s), "hello");
    EXPECT_EQ(TrimLeft("  a"), "a");
    EXPECT_EQ(TrimRight("a  "), "a");

    TrimInPlace(s);
    EXPECT_EQ(s, "hello");
}

TEST(Strings, Case)
{
    EXPECT_EQ(ToLower("AbC"), "abc");
    EXPECT_EQ(ToUpper("AbC"), "ABC");

    std::string s = "AbC";
    ToLowerInPlace(s);
    EXPECT_EQ(s, "abc");
}

TEST(Strings, Predicates)
{
    EXPECT_TRUE(StartsWith("prefix_body", "pre"));
    EXPECT_TRUE(EndsWith("prefix_body", "dy"));
    EXPECT_TRUE(Contains("prefix_body", "x_b"));
}

TEST(Strings, Split)
{
    auto parts = Split("a,b,,c", ',');
    ASSERT_EQ(parts.size(), 3u);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");

    auto views = SplitView("a,,b", ',', false);
    ASSERT_EQ(views.size(), 3u);
    EXPECT_EQ(views[1], "");
}


