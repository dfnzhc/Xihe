include(FetchContent)

# 日志 - spdlog
CPMAddPackage(
        NAME spdlog
        VERSION 1.15.3
        GITHUB_REPOSITORY gabime/spdlog
)

if (NOT spdlog_ADDED)
    message(FATAL_ERROR "Failed to add spdlog library.")
endif ()

add_library(Spdlog INTERFACE)
target_link_libraries(Spdlog INTERFACE spdlog::spdlog)
set_property(TARGET Spdlog PROPERTY FOLDER "External")

# Eigen
CPMAddPackage(
        NAME Eigen
        GIT_TAG 3.4
        GITLAB_REPOSITORY libeigen/eigen
        DOWNLOAD_ONLY YES
)

if (NOT Eigen_ADDED)
    message(FATAL_ERROR "Failed to add eigen library.")
endif ()

add_library(Eigen INTERFACE)
target_include_directories(Eigen INTERFACE ${Eigen_SOURCE_DIR})
#target_compile_definitions(Eigen INTERFACE EIGEN_MPL2_ONLY)

if (XIHE_EIGEN_USE_MKL)
    set(MKL_INTERFACE "lp64")
    find_package(MKL)
    if (MKL_FOUND)
        message(STATUS "MKL found, enabling MKL support for Eigen.")
        target_link_libraries(Eigen INTERFACE MKL::MKL)
        target_compile_definitions(Eigen INTERFACE EIGEN_USE_MKL_ALL)
        target_compile_definitions(Eigen INTERFACE "MKL_LP64=1")
    else ()
        message(WARNING "XIHE_EIGEN_USE_MKL is ON, but Intel MKL was not found. MKL support is disabled.")
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

# mimalloc
CPMAddPackage(
        NAME mimalloc
        GITHUB_REPOSITORY "microsoft/mimalloc"
        GIT_TAG "v3.1.5"
        OPTIONS
        "MI_OVERRIDE ON"
        "MI_BUILD_SHARED OFF"
        "MI_BUILD_STATIC ON"
        "MI_BUILD_TESTS OFF"
        "MI_USE_CXX ON"
)

if (NOT mimalloc_ADDED)
    message(FATAL_ERROR "Failed to add mimalloc library.")
endif ()

add_library(Mimalloc INTERFACE)
target_link_libraries(Mimalloc INTERFACE mimalloc-static)
set_property(TARGET Mimalloc PROPERTY FOLDER "External")

# sdl3
CPMAddPackage(
        NAME sdl3
        GITHUB_REPOSITORY "libsdl-org/SDL"
        GIT_TAG "release-3.2.20"
        OPTIONS
        "SDL_TESTS OFF"
        "SDL_EXAMPLES OFF"
        "SDL_SHARED OFF"
        "SDL_STATIC ON"
        "SDL_DISABLE_INSTALL_DOCS ON"
)

if (NOT sdl3_ADDED)
    message(FATAL_ERROR "Failed to add sdl3 library.")
endif ()

add_library(SDL3 INTERFACE)
target_link_libraries(SDL3 INTERFACE SDL3::SDL3-static)
set_property(TARGET SDL3 PROPERTY FOLDER "External")