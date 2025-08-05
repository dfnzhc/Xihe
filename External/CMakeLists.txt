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