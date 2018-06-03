find_package(Qt5Core)

######## GLM ########
# Used for math
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)
# note: glm provides its own make files, which are not included here to avoid adding additional targets cluttering the ide with unused executable targets

######## glad ########
# OpenGL Loader

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

######## glhelper ########
set(GLHELPER_ADDITIONAL_LIBRARIES glad glm Qt5::Core)
set(GLHELPER_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)
set(GLHELPER_CONFIG_FILE ${GLHELPER_CONFIG_DIR}/glhelperconfig.hpp)
add_subdirectory(glhelper)

######## TinyPLY ########
add_library(tinyply STATIC ${CMAKE_CURRENT_SOURCE_DIR}/tinyply/source/tinyply.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tinyply/source/tinyply.h)
target_include_directories(tinyply PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tinyply/source)
# note: tinyply provides its own make files, which are not included here to avoid adding additional targets cluttering the ide with unused executable targets
