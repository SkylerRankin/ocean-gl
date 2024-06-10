cmake_minimum_required(VERSION 3.10)
set(TARGET ocean-gl)
project(${TARGET} VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Prevents the ZERO_CHECK project from being generated
set(CMAKE_SUPPRESS_REGENERATION true)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include_directories(lib/glfw-3.4/include)
add_subdirectory(lib/glfw-3.4)

# GLAD
include_directories(lib/glad/include)
set(GLAD_SOURCES lib/glad/src/glad.c)

# GLM
include_directories(lib/glm-1.0.1/glm)
add_subdirectory(lib/glm-1.0.1)

# Dear IMGUI
include_directories(lib/imgui-1.90.8)
include_directories(lib/imgui-1.90.8/backends)
file(GLOB_RECURSE IMGUI_SOURCES lib/imgui-1.90.8/*.cpp)
add_library(IMGUI STATIC ${IMGUI_SOURCES})
target_include_directories(IMGUI PUBLIC /lib/imgui-1.90.8)

# List source files, header files, and shader files
set(CXX_SOURCES
    src/main.cpp
    src/engine.cpp
    src/shader.cpp
    src/camera.cpp
    src/ui.cpp
    ${GLAD_SOURCES})

set(CXX_HEADERS
    src/glCommon.h
    src/engine.h
    src/shader.h
    src/camera.h
    src/ui.h)
set_source_files_properties(${CXX_HEADERS} PROPERTIES HEADER_FILE_ONLY true)

set(SHADER_SOURCES
    src/shaders/fragment.glsl
    src/shaders/vertex.glsl)
set_source_files_properties(${SHADER_SOURCES} PROPERTIES HEADER_FILE_ONLY true)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CXX_SOURCES} ${CXX_HEADERS} ${SHADER_SOURCES})

# Copy resources
# When using MSVC, binary is placed into a Debug/Release sub directory, so resources
# should also be copied there.
set(PROJECT_BINARY_SUBDIR "")
if (MSVC)
    set(PROJECT_BINARY_SUBDIR /$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)
endif()
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/src/shaders
    ${PROJECT_BINARY_DIR}${PROJECT_BINARY_SUBDIR}/shaders
    COMMENT "Copying resources directory into build directory")

add_executable(${TARGET} ${CXX_SOURCES} ${CXX_HEADERS} ${GLAD_SOURCES})
target_sources(${TARGET} PRIVATE ${SHADER_SOURCES})
add_dependencies(${TARGET} copy_resources)

# Controls if a command prompt window is opened when running the executable on Windows.
set(HIDE_COMMAND_WINDOW false)
if (${WIN32} AND ${HIDE_COMMAND_WINDOW})
    set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
endif()

# target_compile_options(${TARGET} PUBLIC -Wall)
target_link_libraries(${TARGET} glfw)
target_link_libraries(${TARGET} glm::glm)
target_link_libraries(${TARGET} IMGUI)
