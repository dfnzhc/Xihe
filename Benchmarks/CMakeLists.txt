cmake_minimum_required(VERSION 3.21)

include(FetchContent)

CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        GIT_TAG v1.8.3
        OPTIONS
        "BENCHMARK_ENABLE_TESTING OFF"
)

if (NOT benchmark_ADDED)
    message(FATAL_ERROR "Failed to add google benchmark.")
endif ()

add_library(Benchmark INTERFACE)
target_link_libraries(Benchmark INTERFACE benchmark::benchmark)
set_property(TARGET Benchmark PROPERTY FOLDER "External")

file(GLOB BENCH_SOURCES CONFIGURE_DEPENDS ./*.cpp)
foreach(src ${BENCH_SOURCES})
    AddBenchProgram(${src} "Xihe::Xihe;Benchmark")
endforeach()


