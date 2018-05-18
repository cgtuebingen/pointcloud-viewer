# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.5 REQUIRED)

add_executable(point_cloud_viewer
  camera.cpp
  camera.hpp
  main.cpp
  mainwindow.cpp
  mainwindow.hpp
  viewport.cpp
  viewport.hpp
)

target_link_libraries(point_cloud_viewer Qt5::Widgets glm render_system)
