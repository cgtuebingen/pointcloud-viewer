cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(pointcloud_viewer)

# use c++14
set(CMAKE_CXX_STANDARD 14)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# allow for example `#include <renderer/declarations.hpp>`
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find external dependencies (glm, glfw)
add_subdirectory(external)

# Module containing a collection of utilities/tools/helpers
add_subdirectory(core_library)

# Module containing a collection of geometry functions and structs
add_subdirectory(geometry)

# Module responsible for rendering the points using a render system (OpenGL 4.5)
add_subdirectory(renderer)
# Module loading the points to the acceleration structure and discarding them, when not needed anymore
add_subdirectory(streaming_system)
# The application putting everything together in a gui
add_subdirectory(pointcloud_viewer)

# Unittests
add_subdirectory(tests)
