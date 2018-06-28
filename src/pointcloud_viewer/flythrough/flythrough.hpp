#ifndef POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_

#include <pointcloud_viewer/flythrough/keypoint.hpp>

#include <QAbstractListModel>

class Flythrough : public QAbstractListModel
{
public:
  void insert_keypoint(frame_t frame, int index);

protected:
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  QVector<keypoint_t> _keypoints;
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
