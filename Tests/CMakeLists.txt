cmake_minimum_required(VERSION 3.30)

project(XiheTest LANGUAGES CXX)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
find_package(GTest CONFIG REQUIRED)

set(${CMAKE_CURRENT_BINARY_DIR})


AddTestProgram(Core/ConfigTest.cpp "GTest::gtest;GTest::gtest_main;Xihe::Xihe")