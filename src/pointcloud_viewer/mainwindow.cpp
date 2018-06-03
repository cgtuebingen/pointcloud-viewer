#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
  : pointCloudLayer(this),
    importPointCloud(this)
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  initMenuBar();
  initDocks();
}

MainWindow::~MainWindow()
{
}
