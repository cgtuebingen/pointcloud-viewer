# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.5 REQUIRED)

add_executable(point_cloud_viewer
  workers/import_pointcloud.cpp
  workers/import_pointcloud.hpp
  camera.cpp
  camera.hpp
  declarations.hpp
  main.cpp
  mainwindow.cpp
  mainwindow_docks.cpp
  mainwindow_menubar.cpp
  mainwindow.hpp
  navigation.cpp
  navigation.hpp
  point_cloud_layers.cpp
  point_cloud_layers.hpp
  point_cloud.cpp
  point_cloud.hpp
  viewport.cpp
  viewport.hpp
  visualizations.cpp
  visualizations.hpp
)

target_link_libraries(point_cloud_viewer Qt5::Widgets glm renderer geometry tinyply)
