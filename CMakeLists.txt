cmake_minimum_required(VERSION 3.0.0)
project(ocean-gl VERSION 0.1)

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

add_executable(ocean-gl src/main.cpp lib/glad/src/glad.c)
target_compile_options(ocean-gl PUBLIC -Wall)
target_link_libraries(ocean-gl glfw)
