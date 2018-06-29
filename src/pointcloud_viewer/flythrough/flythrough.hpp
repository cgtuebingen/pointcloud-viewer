#ifndef POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_

#include <pointcloud_viewer/flythrough/keypoint.hpp>
#include <pointcloud_viewer/flythrough/interpolation.hpp>

#include <QAbstractListModel>

class Flythrough : public QAbstractListModel
{
  Q_OBJECT
  Q_PROPERTY(double animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
  Q_PROPERTY(double cameraVelocity READ cameraVelocity WRITE setCameraVelocity NOTIFY cameraVelocityChanged)
  Q_PROPERTY(double pathLength READ pathLength WRITE setPathLength NOTIFY pathLengthChanged)
public:

  Flythrough();
  ~Flythrough() override;

  void insert_keypoint(frame_t frame, int index);

  keypoint_t keypoint_at(int index) const;

  double animationDuration() const;
  double cameraVelocity() const;
  double pathLength() const;

  frame_t camera_position_for_time(double time, frame_t fallback) const;

public slots:
  void setAnimationDuration(double animationDuration);
  void setCameraVelocity(double cameraVelocity);

signals:
  void animationDurationChanged(double animationDuration);
  void cameraVelocityChanged(double cameraVelocity);
  void pathLengthChanged(double pathLength);

protected:
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  QVector<keypoint_t> _keypoints;
  Interpolation* interpolation;

  double m_animationDuration = 10.;
  double m_cameraVelocity = 1.;
  double m_pathLength = 0.;

private slots:
  void setPathLength(double pathLength);

  void updatePathLength();
  void updateCameraVelocits();
  void updateAnimationDuration();
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
