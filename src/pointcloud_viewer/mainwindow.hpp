#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>

#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/point_cloud_layers.hpp>
#include <pointcloud_viewer/workers/import_pointcloud.hpp>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  PointCloudLayers pointCloudLayer;
  ImportPointCloud importPointCloud;

  MainWindow();
  ~MainWindow();

private:
  Viewport viewport;

  void initMenuBar();
  void initDocks();

  void importPointcloudLayer();
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
