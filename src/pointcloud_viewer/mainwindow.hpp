#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>

#include <pointcloud_viewer/viewport.hpp>

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

private:
  Viewport viewport;

  void initMenuBar();
  void initDocks();
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
