#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
  : pointCloudLayer(this)
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  handleApplicationArguments();
  initMenuBar();
  initDocks();

  connect(&flythrough, &Flythrough::set_new_camera_frame, &viewport, &Viewport::set_camera_frame);
  connect(&viewport, &Viewport::frame_rendered, &flythrough.playback, &Playback::previous_frame_finished);
}

MainWindow::~MainWindow()
{
}
