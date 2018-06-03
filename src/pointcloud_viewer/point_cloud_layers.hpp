#ifndef POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_
#define POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_

#include <QAbstractListModel>

/**
This class contains and owns all point-cloud layers.
*/
class PointCloudLayers final : public QAbstractListModel
{
Q_OBJECT
public:

public slots:
  void importPointCloudLayer(QString filename);

protected:
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation, int role) const override;
};

#endif // POINTCLOUDVIEWER_POINT_CLOUD_LAYER_HPP_
