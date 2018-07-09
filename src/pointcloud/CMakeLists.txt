# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core 5.5 REQUIRED)
find_package(ASSIMP 3.2)

add_library(pointcloud STATIC
 helpers/all_buffer_types.inl
 helpers/all_buffer_types_combi.inl
 importer/abstract_importer.cpp
 importer/abstract_importer.hpp
 importer/assimp_importer.cpp
 importer/assimp_importer.hpp
 importer/ply_importer.cpp
 importer/ply_importer.hpp
 buffer.cpp
 buffer.hpp
 convert_values.hpp
 pointcloud.cpp
 pointcloud.hpp
)

target_link_libraries(pointcloud PUBLIC Qt5::Core core_library geometry pcl)

if(ASSIMP_FOUND)
target_compile_definitions(pointcloud PUBLIC "-DUSE_ASSIMP")
target_link_libraries(pointcloud PUBLIC assimp)
endif()
