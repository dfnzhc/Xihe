file(GLOB_RECURSE SOURCE_FILES ./*.hpp ./*.cpp)
AssignSourceGroup(${SOURCE_FILES})

add_executable(XiheEditor ${SOURCE_FILES})

SetCompilerFlags(XiheEditor)
SetDefaultCompileDefinitions(XiheEditor)

target_link_libraries(XiheEditor
        PRIVATE
        Xihe::Xihe
)

target_include_directories(XiheEditor
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(XiheEditor
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${XIHE_BINARY_DIR}
)