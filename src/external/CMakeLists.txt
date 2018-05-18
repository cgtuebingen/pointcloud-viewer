set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

######## GLM ########
# Used for math

set(GLM_STATIC_LIBRARY_ENABLE ON CACHE BOOL "Use a static glm library by default")

add_subdirectory(glm)

######## glad ########
# OpenGL Loader

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

######## glhelper ########
set(GLHELPER_ADDITIONAL_LIBRARIES glad glm)
set(GLHELPER_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(GLHELPER_CONFIG_FILE ${GLHELPER_CONFIG_DIR}/glhelperconfig.hpp)
add_subdirectory(glhelper)

######## TinyPLY ########

# add_subdirectory(tinyply)


