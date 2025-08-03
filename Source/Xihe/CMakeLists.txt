file(GLOB_RECURSE SOURCE_FILES ./*.hpp ./*.cpp)
AssignSourceGroup(${SOURCE_FILES})

add_executable(Xihe ${SOURCE_FILES})

SetCompilerFlags(Xihe)
SetDefaultCompileDefinitions(Xihe)

target_link_libraries(Xihe
        PRIVATE
        Xihe::Xihe
)

target_include_directories(Xihe
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(Xihe
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${XIHE_BINARY_DIR}
)