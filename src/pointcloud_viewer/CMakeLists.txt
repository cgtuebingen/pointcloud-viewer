# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.5 REQUIRED)

add_executable(pointcloud_viewer
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
  pointcloud_layers.cpp
  pointcloud_layers.hpp
  viewport.cpp
  viewport.hpp
  visualizations.cpp
  visualizations.hpp
)

target_link_libraries(pointcloud_viewer Qt5::Widgets glm renderer geometry pointcloud)
