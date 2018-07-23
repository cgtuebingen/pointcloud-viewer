#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>
#include <QListView>
#include <QUrl>
#include <QDropEvent>

#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/pointcloud_layers.hpp>
#include <pointcloud_viewer/flythrough/flythrough.hpp>
#include <pointcloud_viewer/workers/offline_renderer.hpp>

class KeypointList;

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

  RenderSettings renderSettings = RenderSettings::defaultSettings();

  KeypointList* keypointList;

  void handleApplicationArguments();
  void initMenuBar();
  void initDocks();

  void initKeypointListDocks();

  void importPointcloudLayer();
  void openAboutDialog();

  void exportCameraPath();
  void importCameraPath();
  void jumpToKeypoint(const QModelIndex& modelIndex);

  void offline_render_with_ui();
  bool offline_render();

protected:
  void dropEvent(QDropEvent *ev);

  void dragEnterEvent(QDragEnterEvent *ev);
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
