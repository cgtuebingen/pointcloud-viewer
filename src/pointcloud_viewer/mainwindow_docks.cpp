#include <pointcloud_viewer/mainwindow.hpp>

#include <QDockWidget>

void MainWindow::initDocks()
{
  initKeypointListDocks();
}

void MainWindow::initKeypointListDocks()
{
  QDockWidget* dock = new QDockWidget("Flythrough Keypoints", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  keypointList = new QListView;
  keypointList->setModel(&flythrough);

  connect(keypointList, &QListView::doubleClicked, this, &MainWindow::jumpToKeypoint);

  dock->setWidget(keypointList);
}

void MainWindow::jumpToKeypoint(const QModelIndex& modelIndex)
{
  if(!modelIndex.isValid())
    return;

  viewport.set_camera_frame(flythrough.keypoint_at(modelIndex.row()).frame);
}
