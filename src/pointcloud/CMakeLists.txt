# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core 5.5 REQUIRED)

add_library(pointcloud STATIC
 importer/abstract_importer.cpp
 importer/abstract_importer.hpp
 importer/ply_importer.cpp
 importer/ply_importer.hpp
 buffer.cpp
 buffer.hpp
 convert_values.hpp
 kdtree_index.cpp
 kdtree_index.hpp
 pointcloud.cpp
 pointcloud.hpp
)

target_link_libraries(pointcloud PUBLIC boost_sort Qt5::Core core_library geometry pcl)

