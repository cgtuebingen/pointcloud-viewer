#ifndef POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_

#include <pointcloud_viewer/flythrough/keypoint.hpp>

#include <QAbstractListModel>

class Flythrough : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(double animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
public:
  void insert_keypoint(frame_t frame, int index);

  keypoint_t keypoint_at(int index) const;

  double animationDuration() const;

public slots:
  void setAnimationDuration(double animationDuration);

signals:
  void animationDurationChanged(double animationDuration);

protected:
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  QVector<keypoint_t> _keypoints;
  double m_animationDuration = 10.;
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
