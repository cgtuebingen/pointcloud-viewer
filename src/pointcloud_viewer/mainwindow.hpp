#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>

#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/pointcloud_layers.hpp>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  PointCloudLayers pointCloudLayer;

  MainWindow();
  ~MainWindow();

private:
  Viewport viewport;

  void initMenuBar();
  void initDocks();

  void importPointcloudLayer();
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
