#include <pointcloud_viewer/mainwindow.hpp>

#include <QDockWidget>
#include <QListView>

void MainWindow::initDocks()
{
  QDockWidget* dock = new QDockWidget("Flythrough Keypoints", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  QListView* keypointList = new QListView;
  keypointList->setModel(&flythrough);

  dock->setWidget(keypointList);
}
