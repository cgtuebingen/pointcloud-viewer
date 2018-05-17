add_executable(point_cloud_viewer
  clock.cpp
  clock.hpp
  main.cpp
  mainwindow.cpp
  mainwindow.cpp
)

target_link_libraries(point_cloud_viewer glfw glm render_system)
