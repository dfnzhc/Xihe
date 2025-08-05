file(GLOB_RECURSE SOURCE_FILES ./*.hpp ./*.cpp)
AssignSourceGroup(${SOURCE_FILES})

add_library(XiheLib SHARED ${SOURCE_FILES})

SetCompilerFlags(XiheLib)
SetDefaultCompileDefinitions(XiheLib)

set_target_properties(XiheLib
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${XIHE_BINARY_DIR})

target_link_libraries(XiheLib
        PUBLIC
        Spdlog
)

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
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Platform
        ${CMAKE_CURRENT_SOURCE_DIR}/Renderer
)

add_library(Xihe::Xihe ALIAS XiheLib)