# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core 5.5 REQUIRED)

add_library(pointcloud STATIC
 exporter/abstract_exporter.cpp
 exporter/abstract_exporter.hpp
 exporter/ply_exporter.cpp
 exporter/ply_exporter.hpp
 importer/abstract_importer.cpp
 importer/abstract_importer.hpp
 importer/ply_importer.cpp
 importer/ply_importer.hpp
 importer/pcvd_importer.cpp
 importer/pcvd_importer.hpp
 buffer.cpp
 buffer.hpp
 buffer.inl
 convert_values.hpp
 pcvd_file_format.hpp
 kdtree_index.cpp
 kdtree_index.hpp
 pointcloud.cpp
 pointcloud.hpp
)

target_link_libraries(pointcloud PUBLIC boost_sort Qt5::Core core_library geometry pcl)

