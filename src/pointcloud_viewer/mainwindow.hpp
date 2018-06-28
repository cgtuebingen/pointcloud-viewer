#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>

#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/pointcloud_layers.hpp>
#include <pointcloud_viewer/flythrough/flythrough.hpp>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  PointCloudLayers pointCloudLayer;

  MainWindow();
  ~MainWindow();

private:
  Viewport viewport;
  Flythrough flythrough;

  void initMenuBar();
  void initDocks();

  void importPointcloudLayer();
  void openAboutDialog();

  void insertKeypoint();
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
