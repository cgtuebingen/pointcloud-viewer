######## GLFW ########

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build the GLFW examples by default")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build the GLFW tests by default")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build the GLFW docs by default")

add_subdirectory(glfw)

######## GLM ########

set(GLM_STATIC_LIBRARY_ENABLE ON CACHE BOOL "Use a static glm library by default")

add_subdirectory(glm)

######## TinyPLY ########

# add_subdirectory(tinyply)

