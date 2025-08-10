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

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ./*.cpp)
foreach (src ${TEST_SOURCES})
    AddTestProgram(${src} "Xihe::Xihe;GTest::gtest;GTest::gtest_main;GTest::gmock")
endforeach ()
