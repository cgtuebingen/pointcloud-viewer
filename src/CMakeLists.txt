cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(pointcloud_viewer)

# use c++14
set(CMAKE_CXX_STANDARD 14)

# allow for example `#include <render_system/instance.hpp>`
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find external dependencies (glm, glfw)
add_subdirectory(external)

# Module responsible for rendering the points using a render system (OpenGL 4.5)
add_subdirectory(render_system)
# Module loading the points to the acceleration structure and discarding them, when not needed anymore
add_subdirectory(streaming_system)
# The application putting everything together in a gui
add_subdirectory(pointcloud_viewer)

# Unittests
add_subdirectory(tests)
