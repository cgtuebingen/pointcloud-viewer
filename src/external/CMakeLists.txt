set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

######## GLFW ########
# application window and input management

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the GLFW examples by default")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the GLFW tests by default")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build the GLFW docs by default")

add_subdirectory(glfw)

######## GLM ########
# Used for math

set(GLM_STATIC_LIBRARY_ENABLE ON CACHE BOOL "Use a static glm library by default")

add_subdirectory(glm)

######## glad ########
# OpenGL Loader

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

######## TinyPLY ########

# add_subdirectory(tinyply)


