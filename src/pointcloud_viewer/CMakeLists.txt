add_executable(point_cloud_viewer
  main.cpp
  mainwindow.cpp
  mainwindow.cpp
)

target_link_libraries(point_cloud_viewer glfw glm render_system)
