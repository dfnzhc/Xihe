/**
 * @File EnumTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/3
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <Core/Utils/Enum.hpp>

using namespace xihe;

enum class Permissions : unsigned int
{
    None    = 0,
    Read    = 1 << 0,                // 1
    Write   = 1 << 1,                // 2
    Execute = 1 << 2,                // 4
    All     = Read | Write | Execute // 7
};

XIHE_ENUM_CLASS_OPERATORS(Permissions);

TEST(EnumClassOperatorsTest, BitwiseAnd)
{
    EXPECT_EQ(Permissions::Read & Permissions::Write, Permissions::None);

    auto read_write = Permissions::Read | Permissions::Write;
    EXPECT_EQ(read_write & Permissions::Read, Permissions::Read);
    EXPECT_EQ(read_write & Permissions::Write, Permissions::Write);
    EXPECT_EQ(read_write & Permissions::Execute, Permissions::None);

    EXPECT_EQ(Permissions::Execute & Permissions::Execute, Permissions::Execute);
}

TEST(EnumClassOperatorsTest, BitwiseOr)
{
    auto read_write = Permissions::Read | Permissions::Write;
    EXPECT_EQ(static_cast<unsigned int>(read_write), 3);

    auto all_perms = read_write | Permissions::Execute;
    EXPECT_EQ(all_perms, Permissions::All);

    EXPECT_EQ(Permissions::Read | Permissions::None, Permissions::Read);
}

TEST(EnumClassOperatorsTest, BitwiseOrAssignment)
{
    auto current_perms = Permissions::Read;

    current_perms |= Permissions::Write;
    EXPECT_EQ(current_perms, Permissions::Read | Permissions::Write);

    current_perms |= Permissions::Read;
    EXPECT_EQ(current_perms, Permissions::Read | Permissions::Write);
}

TEST(EnumClassOperatorsTest, BitwiseAndAssignment)
{
    auto current_perms = Permissions::All;

    current_perms &= (Permissions::Read | Permissions::Execute);
    EXPECT_EQ(current_perms, Permissions::Read | Permissions::Execute);

    current_perms &= Permissions::Execute;
    EXPECT_EQ(current_perms, Permissions::Execute);
}

TEST(EnumClassOperatorsTest, BitwiseNot)
{
    auto read_write = Permissions::Read | Permissions::Write;

    auto should_be_write = read_write & (~Permissions::Read);
    EXPECT_EQ(should_be_write, Permissions::Write);

    auto should_be_read_execute = Permissions::All & (~Permissions::Write);
    EXPECT_EQ(should_be_read_execute, Permissions::Read | Permissions::Execute);
}

TEST(EnumClassOperatorsTest, IsSet)
{
    auto read_write = Permissions::Read | Permissions::Write;

    EXPECT_TRUE(IsSet(read_write, Permissions::Read));
    EXPECT_TRUE(IsSet(read_write, Permissions::Write));

    EXPECT_FALSE(IsSet(read_write, Permissions::Execute));

    EXPECT_TRUE(IsSet(Permissions::Read, Permissions::Read));
    EXPECT_FALSE(IsSet(Permissions::Read, Permissions::Write));

    EXPECT_FALSE(IsSet(Permissions::None, Permissions::Read));
}

TEST(EnumClassOperatorsTest, FlipEnumBit)
{
    auto current_perms = Permissions::Write;

    FlipEnumBit(current_perms, Permissions::Read);
    EXPECT_TRUE(IsSet(current_perms, Permissions::Read));
    EXPECT_TRUE(IsSet(current_perms, Permissions::Write));
    EXPECT_EQ(current_perms, Permissions::Read | Permissions::Write);

    FlipEnumBit(current_perms, Permissions::Write);
    EXPECT_TRUE(IsSet(current_perms, Permissions::Read));
    EXPECT_FALSE(IsSet(current_perms, Permissions::Write));
    EXPECT_EQ(current_perms, Permissions::Read);

    FlipEnumBit(current_perms, Permissions::Write);
    EXPECT_EQ(current_perms, Permissions::Read | Permissions::Write);
}
