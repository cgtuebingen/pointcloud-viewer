find_package(Qt5Core)
find_package(Threads)

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

######## pcl ########
add_subdirectory(pcl)

######## boost sort ########
# Boost

add_library(boost_sort INTERFACE)
target_include_directories(boost_sort INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/boost_sort/include)
target_link_libraries(boost_sort INTERFACE ${CMAKE_THREAD_LIBS_INIT})
