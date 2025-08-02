/**
 * @File Main.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/2
 * @Brief This file is part of Xihe.
 */

#include <iostream>
#include <Core/Services/Logger.hpp>

using namespace xihe;

int main()
{
    Logger logger;
    logger.startup();
    {
        logger.log(Logger::Core, Logger::Trace, "你好 Trace.");
        logger.log(Logger::Core, Logger::Info, "你好 Info.");
        logger.log(Logger::Core, Logger::Warn, "你好 Warning.");
        logger.log(Logger::Core, Logger::Error, "你好 Error.");
        logger.log(Logger::Core, Logger::Fatal, "你好 Fatal.");
    
        logger.log(Logger::Client, Logger::Trace, "你好 Trace.");
        logger.log(Logger::Client, Logger::Info, "你好 Info.");
        logger.log(Logger::Client, Logger::Warn, "你好 Warning.");
        logger.log(Logger::Client, Logger::Error, "你好 Error.");
        logger.log(Logger::Client, Logger::Fatal, "你好 Fatal.");

    }
    logger.shutdown();
    
    std::cout << "羲和" << std::endl;

    
}