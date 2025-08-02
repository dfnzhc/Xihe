file(GLOB_RECURSE SOURCE_FILES ./*.hpp ./*.cpp)
AssignSourceGroup(${SOURCE_FILES})

add_library(XiheLib SHARED ${SOURCE_FILES})

SetCompilerFlags(XiheLib)
SetDefaultCompileDefinitions(XiheLib)

# 外部依赖
add_dependencies(XiheLib spdlog)

set_target_properties(XiheLib
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${XIHE_BINARY_DIR})

target_compile_options(XiheLib
        PUBLIC
        # MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/DEBUG /Zc:preprocessor>
)

target_compile_features(XiheLib
        PUBLIC
        cxx_std_23
)

target_compile_definitions(XiheLib
        PUBLIC
        "$<$<CONFIG:Debug,RelWithDebInfo>:XIHE_ENABLE_DEBUG_BREAK>"
        PRIVATE
        XIHE_DLL
)

target_include_directories(XiheLib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${XIHE_EXTERNAL_LIB_OUTPUT_DIRECTORY}/include/
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform
        ${CMAKE_CURRENT_SOURCE_DIR}/Renderer
)

add_library(Xihe::Xihe ALIAS XiheLib)