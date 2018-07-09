#ifndef POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_
#define POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_

#include <QAbstractListModel>
#include <QMainWindow>

class MainWindow;

/**
This class contains and owns all point-cloud layers.
*/
class PointCloudLayers final : public QAbstractListModel
{
Q_OBJECT
public:
  PointCloudLayers(MainWindow* mainWindow);

protected:
  // Overriding QAbstractListModel methods
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation, int role) const override;

private:
  MainWindow* const mainWindow;
};

#endif // POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_
