# For subclassing QObject
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.5 REQUIRED)

add_executable(pointcloud_viewer
  flythrough/flythrough.cpp
  flythrough/flythrough.hpp
  flythrough/flythrough_file.cpp
  flythrough/flythrough_file.hpp
  flythrough/interpolation.cpp
  flythrough/interpolation.hpp
  flythrough/keypoint.cpp
  flythrough/keypoint.hpp
  flythrough/playback.cpp
  flythrough/playback.hpp
  workers/import_pointcloud.cpp
  workers/import_pointcloud.hpp
  workers/kdtree_builder_dialog.cpp
  workers/kdtree_builder_dialog.hpp
  workers/offline_renderer.cpp
  workers/offline_renderer.hpp
  workers/offline_renderer_dialogs.cpp
  workers/offline_renderer_dialogs.hpp
  camera.cpp
  camera.hpp
  declarations.hpp
  kdtree_inspector.cpp
  kdtree_inspector.hpp
  keypoint_list.cpp
  keypoint_list.hpp
  main.cpp
  mainwindow.cpp
  mainwindow_arguments.cpp
  mainwindow_docks.cpp
  mainwindow_menubar.cpp
  mainwindow.hpp
  navigation.cpp
  navigation.hpp
  version_text.cpp
  version_text.hpp
  viewport.cpp
  viewport.hpp
  visualizations.cpp
  visualizations.hpp
)

target_link_libraries(pointcloud_viewer Qt5::Widgets glm renderer geometry pointcloud)
