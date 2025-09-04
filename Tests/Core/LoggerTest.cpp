/**
 * @File LoggerTest.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include <gtest/gtest.h>
#include <Core/Utils/Logger.hpp>

using namespace xihe;

class LoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Logger::GetInstance().startup();
    }

    void TearDown() override
    {
        Logger::GetInstance().shutdown();
    }
};


TEST_F(LoggerTest, BasicLog)
{
    Logger::GetInstance().log(Logger::Core, Logger::Trace, "你好 Trace.");
    Logger::GetInstance().log(Logger::Core, Logger::Info, "你好 Info.");
    Logger::GetInstance().log(Logger::Core, Logger::Warn, "你好 Warning.");
    Logger::GetInstance().log(Logger::Core, Logger::Error, "你好 Error.");
    Logger::GetInstance().log(Logger::Core, Logger::Fatal, "你好 Fatal.");

    Logger::GetInstance().log(Logger::Client, Logger::Trace, "你好 Trace.");
    Logger::GetInstance().log(Logger::Client, Logger::Info, "你好 Info.");
    Logger::GetInstance().log(Logger::Client, Logger::Warn, "你好 Warning.");
    Logger::GetInstance().log(Logger::Client, Logger::Error, "你好 Error.");
    Logger::GetInstance().log(Logger::Client, Logger::Fatal, "你好 Fatal.");
}

TEST_F(LoggerTest, Log)
{
    Log(Logger::Core, Logger::Trace, "你好 Trace.");
    Log(Logger::Core, Logger::Info, "你好 Info.");
    Log(Logger::Core, Logger::Warn, "你好 Warning.");
    Log(Logger::Core, Logger::Error, "你好 Error.");
    Log(Logger::Core, Logger::Fatal, "你好 Fatal.");

    Log(Logger::Client, Logger::Trace, "你好 Trace.");
    Log(Logger::Client, Logger::Info, "你好 Info.");
    Log(Logger::Client, Logger::Warn, "你好 Warning.");
    Log(Logger::Client, Logger::Error, "你好 Error.");
    Log(Logger::Client, Logger::Fatal, "你好 Fatal.");
}

TEST_F(LoggerTest, LogWithArgs)
{
    int a         = 1;
    float b       = 2.f;
    double c      = 3.;
    const char* d = "4";
    Log(Logger::Core, Logger::Trace, "你好 Trace: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Info, "你好 Info: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Warn, "你好 Warning: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Error, "你好 Error: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Fatal, "你好 Fatal: {}.{}.{}.{}", a, b, c, d);

    Log(Logger::Client, Logger::Trace, "你好 Trace: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Info, "你好 Info: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Warn, "你好 Warning: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Error, "你好 Error: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Fatal, "你好 Fatal: {}.{}.{}.{}", a, b, c, d);
}

TEST_F(LoggerTest, LogWithLocals)
{
    int a         = 1;
    float b       = 2.f;
    double c      = 3.;
    const char* d = "4";

    Log(Logger::Core, Logger::Trace, std::source_location::current(), "你好 Trace: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Info, std::source_location::current(), "你好 Info: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Warn, std::source_location::current(), "你好 Warning: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Error, std::source_location::current(), "你好 Error: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Core, Logger::Fatal, std::source_location::current(), "你好 Fatal: {}.{}.{}.{}", a, b, c, d);

    Log(Logger::Client, Logger::Trace, std::source_location::current(), "你好 Trace: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Info, std::source_location::current(), "你好 Info: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Warn, std::source_location::current(), "你好 Warning: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Error, std::source_location::current(), "你好 Error: {}.{}.{}.{}", a, b, c, d);
    Log(Logger::Client, Logger::Fatal, std::source_location::current(), "你好 Fatal: {}.{}.{}.{}", a, b, c, d);
}

TEST_F(LoggerTest, LogWithIndenter)
{
    Log(Logger::Core, Logger::Info, std::source_location::current(), "0: Level: {}", LogIndenter::GetLevel());

    {
        XIHE_LOG_SCOPE();
        Log(Logger::Core, Logger::Info, std::source_location::current(), "1: Level: {}",
            LogIndenter::GetLevel());
        {
            XIHE_LOG_SCOPE();

            Log(Logger::Core, Logger::Info, std::source_location::current(), "2: Level: {}",
                LogIndenter::GetLevel());
            {
                XIHE_LOG_SCOPE();
                Log(Logger::Core, Logger::Info, std::source_location::current(), "3: Level: {}",
                    LogIndenter::GetLevel());
            }
            Log(Logger::Core, Logger::Info, std::source_location::current(), "4: Level: {}",
                LogIndenter::GetLevel());
        }
        Log(Logger::Core, Logger::Info, std::source_location::current(), "5: Level: {}",
            LogIndenter::GetLevel());
    }
    Log(Logger::Core, Logger::Info, std::source_location::current(), "6: Level: {}", LogIndenter::GetLevel());
}

TEST_F(LoggerTest, LogUseMacros)
{
    int a         = 1;
    float b       = 2.f;
    double c      = 3.;
    const char* d = "4";

    XIHE_CORE_TRACE("你好 Trace: {}.{}.{}.{}", a, b, c, d);
    XIHE_CORE_INFO("你好 Info: {}.{}.{}.{}", a, b, c, d);
    XIHE_CORE_WARN("你好 Warning: {}.{}.{}.{}", a, b, c, d);
    XIHE_CORE_ERROR("你好 Error: {}.{}.{}.{}", a, b, c, d);
    XIHE_CORE_FATAL("你好 Fatal: {}.{}.{}.{}", a, b, c, d);

    XIHE_TRACE("你好 Trace: {}.{}.{}.{}", a, b, c, d);
    XIHE_INFO("你好 Info: {}.{}.{}.{}", a, b, c, d);
    XIHE_WARN("你好 Warning: {}.{}.{}.{}", a, b, c, d);
    XIHE_ERROR("你好 Error: {}.{}.{}.{}", a, b, c, d);
    XIHE_FATAL("你好 Fatal: {}.{}.{}.{}", a, b, c, d);
}
