cmake_minimum_required(VERSION 3.30)

project(XiheTest LANGUAGES CXX)

CPMAddPackage(
        NAME gtest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.17.0
        VERSION 1.17.0
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

set(${CMAKE_CURRENT_BINARY_DIR})

AddTestProgram(Core/ErrorTest.cpp "GTest::gtest;GTest::gtest_main;GTest::gmock;Xihe::Xihe")
AddTestProgram(Core/ConfigTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")

AddTestProgram(Core/LoggerTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")
AddTestProgram(Core/EnumTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")

AddTestProgram(Math/CommonTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")
AddTestProgram(Math/RandomTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")

AddTestProgram(Core/TimeTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")

AddTestProgram(Core/EventBusTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")
AddTestProgram(Core/StringsTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")