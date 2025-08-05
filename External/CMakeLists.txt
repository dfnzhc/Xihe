include(FetchContent)

# 日志 - spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.15.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(spdlog)

add_library(Spdlog INTERFACE)
target_link_libraries(Spdlog INTERFACE spdlog::spdlog)
set_property(TARGET Spdlog PROPERTY FOLDER "External")

# Eigen
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_DEMOS OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_EXAMPLES OFF)
FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(eigen)

add_library(Eigen INTERFACE)
target_link_libraries(Eigen INTERFACE Eigen3::Eigen)

target_compile_definitions(Eigen INTERFACE EIGEN_MPL2_ONLY)

if (XIHE_EIGEN_USE_MKL)
    find_package(MKL)
    if (MKL_FOUND)
        message(STATUS "MKL found, enabling MKL support for Eigen.")
        target_include_directories(Eigen INTERFACE ${MKL_INCLUDE_DIRS})
        target_link_libraries(Eigen INTERFACE ${MKL_LIBRARIES})
        target_compile_definitions(Eigen INTERFACE EIGEN_USE_MKL_ALL)
    else ()
        message(WARNING "SLIB_EIGEN_USE_MKL is ON, but Intel MKL was not found. MKL support is disabled.")
    endif ()
endif ()

if (XIHE_EIGEN_ENABLE_SIMD)
    include(CheckCXXCompilerFlag)

    # For GCC/Clang
    check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if (COMPILER_SUPPORTS_MARCH_NATIVE)
        message(STATUS "Enabling native SIMD optimizations with -march=native.")
        target_compile_options(Eigen INTERFACE "-march=native")
    else ()
        # For MSVC
        check_cxx_compiler_flag("/arch:AVX2" COMPILER_SUPPORTS_ARCH_AVX2)
        if (COMPILER_SUPPORTS_ARCH_AVX2)
            message(STATUS "Enabling AVX2 SIMD optimizations with /arch:AVX2.")
            target_compile_options(Eigen INTERFACE "/arch:AVX2")
        else ()
            message(WARNING "Could not automatically enable advanced SIMD for this compiler. You may need to set compiler flags manually.")
        endif ()
    endif ()
endif ()


set_property(TARGET Eigen PROPERTY FOLDER "External")