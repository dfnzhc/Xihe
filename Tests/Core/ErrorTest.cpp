/**
 * @File ErrorTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#ifdef XIHE_ENABLE_DEBUG_BREAK
#undef XIHE_ENABLE_DEBUG_BREAK
#define XIHE_ENABLE_DEBUG_BREAK 0
#endif
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
    xihe::Exception e2(e1);
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

TEST(ThrowMacros, XIHE_THROW_WithMessage)
{
    try { XIHE_THROW("Simple error message"); } catch (const xihe::RuntimeError& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("Simple error message"));
        EXPECT_THAT(e.what(), ::testing::HasSubstr("ErrorTest.cpp"));
    }
}

TEST(ThrowMacros, XIHE_THROW_WithFormat)
{
    try {
        int error_code = 404;
        XIHE_THROW("Formatted error with code: {}", error_code);
    } catch (const xihe::RuntimeError& e) { EXPECT_THAT(e.what(), ::testing::HasSubstr("Formatted error with code: 404")); }
}

TEST(ThrowMacros, XIHE_THROW_WithChineseMessage)
{
    try { XIHE_THROW("这是一个中文错误"); } catch (const xihe::RuntimeError& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("这是一个中文错误"));
    }
}

TEST(CheckMacros, XIHE_CHECK_Succeeds)
{
    EXPECT_NO_THROW(XIHE_CHECK(true, "This should not throw"));
    EXPECT_NO_THROW(XIHE_CHECK(1 == 1, "This should not throw"));
}

TEST(CheckMacros, XIHE_CHECK_Fails)
{
    EXPECT_THROW(XIHE_CHECK(false, "This should throw"), xihe::RuntimeError);
}

TEST(AssertMacros, XIHE_ASSERT_Succeeds)
{
    EXPECT_NO_THROW(XIHE_ASSERT(true, "This should not throw"));
    EXPECT_NO_THROW(XIHE_ASSERT(2 > 1, "This should not throw"));
}

TEST(AssertMacros, XIHE_ASSERT_FailsWithMessage)
{
    try { XIHE_ASSERT(1 > 2, "A custom message."); } catch (const xihe::AssertionError& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("1 > 2"));
        EXPECT_THAT(e.what(), ::testing::HasSubstr("A custom message."));
    }
}

TEST(AssertMacros, XIHE_ASSERT_FailsWithChineseMessageAndFormat)
{
    try {
        int value = 100;
        XIHE_ASSERT(value < 0, "值 {} 必须为负数", value);
    } catch (const xihe::AssertionError& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("value < 0"));
        EXPECT_THAT(e.what(), ::testing::HasSubstr("值 100 必须为负数"));
    }
}

TEST(AssertOpMacros, XIHE_ASSERT_EQ_Fails)
{
    int x = 5;
    int y = 10;
    try { XIHE_ASSERT_EQ(x, y); } catch (const xihe::AssertionError& e) {
        EXPECT_THAT(e.what(), ::testing::HasSubstr("x == y (5 == 10)"));
    }
}

TEST(AssertOpMacros, XIHE_ASSERT_NE_Succeeds)
{
    int x = 5;
    int y = 10;
    EXPECT_NO_THROW(XIHE_ASSERT_NE(x, y));
}

TEST(DebugAssertMacros, Behavior)
{
    EXPECT_THROW(XIHE_DEBUG_ASSERT(false, "Should fail in debug mode"), xihe::AssertionError);
    std::cout << "Testing in DEBUG mode: XIHE_DEBUG_ASSERT is active." << std::endl;
}

TEST(GuardianFunc, ReturnsTrueOnSuccessVoid)
{
    bool result = Guardian([] { });
    EXPECT_TRUE(result);
}

TEST(GuardianFunc, ReturnsCorrectBool)
{
    bool result_true = Guardian([] { return true; });
    bool result_false = Guardian([] { return false; });
    EXPECT_TRUE(result_true);
    EXPECT_FALSE(result_false);
}

TEST(GuardianFunc, CatchesExceptionAndReturnsFalse)
{
    testing::internal::CaptureStderr();

    bool result = Guardian([] { XIHE_THROW("Guardian test"); });

    std::string output = testing::internal::GetCapturedStderr();

    EXPECT_FALSE(result);
    EXPECT_THAT(output, ::testing::HasSubstr("Exception:"));
    EXPECT_THAT(output, ::testing::HasSubstr("Guardian test"));
}