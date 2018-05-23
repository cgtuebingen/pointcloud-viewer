#include <pointcloud_viewer/mainwindow.hpp>

MainWindow::MainWindow()
{
  setWindowTitle("Pointcloud Viewer");

  setCentralWidget(&viewport);

  initMenuBar();
}

MainWindow::~MainWindow()
{
}
