#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
  : kdTreeInspector(this),
    pointCloudInspector(&viewport)
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  initMenuBar();
  initDocks();

  connect(&flythrough, &Flythrough::set_new_camera_frame, &viewport, &Viewport::set_camera_frame);
  connect(&viewport, &Viewport::frame_rendered, &flythrough.playback, &Playback::previous_frame_finished);
  connect(&viewport, &Viewport::openGlContextCreated, this, &MainWindow::handleApplicationArguments);

  connect(&viewport.navigation, &Navigation::simpleLeftClick, &pointCloudInspector, &PointCloudInspector::pick_point, Qt::QueuedConnection);

  connect(this, &MainWindow::pointcloud_unloaded, [this](){pointcloud.clear();});
  connect(this, &MainWindow::pointcloud_unloaded, &viewport, &Viewport::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &kdTreeInspector, &KdTreeInspector::unload_all_point_clouds);
  connect(this, &MainWindow::pointcloud_unloaded, &pointCloudInspector, &PointCloudInspector::unload_all_point_clouds);

  connect(this, &MainWindow::pointcloud_imported, [this](QSharedPointer<PointCloud> p){pointcloud = p;});
  connect(this, &MainWindow::pointcloud_imported, &viewport, &Viewport::load_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &kdTreeInspector, &KdTreeInspector::handle_new_point_cloud);
  connect(this, &MainWindow::pointcloud_imported, &pointCloudInspector, &PointCloudInspector::handle_new_point_cloud);
}

MainWindow::~MainWindow()
{
}
