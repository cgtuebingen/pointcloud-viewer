#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <QMainWindow>
#include <QListView>
#include <QUrl>
#include <QDropEvent>

#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/kdtree_inspector.hpp>
#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud_viewer/point_shader_editor.hpp>
#include <pointcloud_viewer/flythrough/flythrough.hpp>
#include <pointcloud_viewer/workers/offline_renderer.hpp>

class KeypointList;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
  bool noninteractive = false;

  MainWindow();
  ~MainWindow();

  bool apply_point_shader(PointCloud::Shader new_shader);

signals:
  void pointcloud_imported(QSharedPointer<PointCloud> point_cloud);
  void pointcloud_unloaded();

private:
  Viewport viewport;
  Flythrough flythrough;
  KdTreeInspector kdTreeInspector;
  PointCloudInspector pointCloudInspector;
  PointShaderEditor pointShaderEditor;

  RenderSettings renderSettings = RenderSettings::defaultSettings();

  KeypointList* keypointList;

  void handleApplicationArguments();
  void initMenuBar();
  void initDocks();

  QDockWidget* initAnimationDock();
  QDockWidget* initRenderDock();
  QDockWidget* initDataInspectionDock();

  void importPointcloudLayer();
  void exportPointcloud();
  void openAboutDialog();

  void exportCameraPath();
  void importCameraPath();
  void jumpToKeypoint(const QModelIndex& modelIndex);

  void offline_render_with_ui();
  bool offline_render();

protected:
  void dropEvent(QDropEvent *ev);

  void dragEnterEvent(QDragEnterEvent *ev);

  void closeEvent(QCloseEvent* event);

private:
  QSharedPointer<PointCloud> pointcloud;
  PointCloud::Shader loadedShader;

  void import_pointcloud(QString filepath);
  void export_pointcloud(QString filepath, QString selectedFilter);
};


#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
