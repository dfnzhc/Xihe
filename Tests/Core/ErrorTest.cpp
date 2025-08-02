/**
 * @File ErrorTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <Core/Base/Error.hpp>

using namespace xihe;

class ExceptionTest : public ::testing::Test
{
protected:
    const char* AsciiMsg = "A normal test message.";
    const char* ChineseMsg = "这是一个包含中文的错误信息";
};

TEST_F(ExceptionTest, DefaultConstructor)
{
    xihe::Exception e;
    EXPECT_STREQ("", e.what());
}

TEST_F(ExceptionTest, AsciiConstructorAndWhat)
{
    xihe::Exception e(AsciiMsg);
    EXPECT_STREQ(AsciiMsg, e.what());
}

TEST_F(ExceptionTest, ChineseConstructorAndWhat)
{
    xihe::Exception e(ChineseMsg);
    EXPECT_STREQ(ChineseMsg, e.what());
}

TEST_F(ExceptionTest, CopyConstructor)
{
    xihe::Exception e1(ChineseMsg);
    xihe::Exception e2(e1); // 执行拷贝
    EXPECT_STREQ(ChineseMsg, e2.what());
}

TEST_F(ExceptionTest, RuntimeErrorThrowAndCatch)
{
    EXPECT_THROW({
                 throw xihe::RuntimeError(ChineseMsg);
                 },
                 xihe::RuntimeError);

    try { throw xihe::RuntimeError(ChineseMsg); } catch (const xihe::RuntimeError& e) { EXPECT_STREQ(ChineseMsg, e.what()); } catch (...) {
        FAIL() << "Expected to catch xihe::RuntimeError but caught something else.";
    }
}

TEST_F(ExceptionTest, AssertionErrorThrowAndCatch)
{
    EXPECT_THROW({
                 throw xihe::AssertionError(AsciiMsg);
                 },
                 xihe::AssertionError);

    try { throw xihe::AssertionError(ChineseMsg); } catch (const xihe::AssertionError& e) { EXPECT_STREQ(ChineseMsg, e.what()); } catch (...) {
        FAIL() << "Expected to catch xihe::AssertionError but caught something else.";
    }
}

TEST_F(ExceptionTest, PolymorphicCatching)
{
    try { throw xihe::RuntimeError(ChineseMsg); } catch (const xihe::Exception& e) {
        SUCCEED() << "Correctly caught RuntimeError as a base xihe::Exception.";
        EXPECT_STREQ(ChineseMsg, e.what());
    } catch (...) { FAIL() << "Expected to catch xihe::Exception."; }

    try { throw xihe::AssertionError(AsciiMsg); } catch (const std::exception& e) {
        SUCCEED() << "Correctly caught AssertionError as a standard std::exception.";
        EXPECT_STREQ(AsciiMsg, e.what());
    } catch (...) { FAIL() << "Expected to catch std::exception."; }
}