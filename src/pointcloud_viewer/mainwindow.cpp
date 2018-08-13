#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  initMenuBar();
  initDocks();

  connect(&flythrough, &Flythrough::set_new_camera_frame, &viewport, &Viewport::set_camera_frame);
  connect(&viewport, &Viewport::frame_rendered, &flythrough.playback, &Playback::previous_frame_finished);
  connect(&viewport, &Viewport::openGlContextCreated, this, &MainWindow::handleApplicationArguments);

  connect(this, &MainWindow::pointcloud_unloaded, &viewport, &Viewport::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &pointDataInspector, &PointCloudInspector::unload_all_point_clouds);

  connect(this, &MainWindow::pointcloud_imported, this, &MainWindow::pointcloud_unloaded);
  connect(this, &MainWindow::pointcloud_imported, &viewport, &Viewport::load_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &pointDataInspector, &PointCloudInspector::handle_new_point_cloud);
}

MainWindow::~MainWindow()
{
}
