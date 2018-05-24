#include <pointcloud_viewer/mainwindow.hpp>

#include <QDockWidget>
#include <QListWidget> // TODO remove

void MainWindow::initDocks()
{
  QDockWidget* dock = new QDockWidget("Pointcloud Layers", this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  addDockWidget(Qt::LeftDockWidgetArea, dock);

  QListWidget* dummyWidget = new QListWidget;
  dock->setWidget(dummyWidget);
}
