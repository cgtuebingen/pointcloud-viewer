#include <pointcloud_viewer/point_cloud_layers.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <QDebug>

PointCloudLayers::PointCloudLayers(MainWindow* mainWindow)
  : mainWindow(mainWindow)
{
}

enum Section
{
  SECTION_VISIBILITY,
};

int PointCloudLayers::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 0; // TODO
}

QVariant PointCloudLayers::data(const QModelIndex& index, int role) const
{
  switch(role)
  {
  case Qt::DisplayRole:
  switch(index.column())
  {
  case SECTION_VISIBILITY:
    return true; // TODO
  default:
    return QVariant();
  }
  default:
    return QVariant();
  }
}

QVariant PointCloudLayers::headerData(int section, Qt::Orientation, int role) const
{
  switch(role)
  {
  case Qt::DisplayRole:
    switch(section)
    {
    case SECTION_VISIBILITY:
      return QVariant();
    default:
      return QVariant();
    }
  default:
    return QVariant();
  }
}
