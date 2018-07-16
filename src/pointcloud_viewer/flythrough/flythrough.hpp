#ifndef POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
#define POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_

#include <pointcloud_viewer/flythrough/keypoint.hpp>
#include <pointcloud_viewer/flythrough/interpolation.hpp>
#include <pointcloud_viewer/flythrough/playback.hpp>

#include <QAbstractListModel>
#include <QSharedPointer>

/** Contains the path of the camera and allows accessing the camera frame for any point in time.
*/
class Flythrough : public QAbstractListModel
{
  Q_OBJECT
public:
  Q_PROPERTY(double animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
  Q_PROPERTY(double cameraVelocity READ cameraVelocity WRITE setCameraVelocity NOTIFY cameraVelocityChanged)
  Q_PROPERTY(double pathLength READ pathLength WRITE setPathLength NOTIFY pathLengthChanged)
  Q_PROPERTY(bool canPlay READ canPlay WRITE setCanPlay NOTIFY canPlayChanged)
  Q_PROPERTY(int interpolation READ interpolation WRITE setInterpolation NOTIFY interpolationChanged)

public:
  enum interpolation_t
  {
    INTERPOLATION_LINEAR,
    INTERPOLATION_LINEAR_SMOOTHSTEP,

    INTERPOLATION_NUMBER_VALUES,
  };

  Playback playback;

  Flythrough(const Flythrough& flythrough);

  Flythrough();
  ~Flythrough() override;

  QSharedPointer<Flythrough> copy() const;

  void insert_keypoint(frame_t frame, int index);

  keypoint_t keypoint_at(int index) const;
  const QVector<keypoint_t>& all_keypoints() const;

  double animationDuration() const;
  double cameraVelocity() const;
  double pathLength() const;

  frame_t camera_position_for_time(double time, frame_t fallback) const;

  bool canPlay() const;

  int interpolation() const;

  void export_path(QString filepath) const;
  void import_path(QString filepath);

public slots:
  void setAnimationDuration(double animationDuration);
  void setCameraVelocity(double cameraVelocity);
  void setInterpolation(int interpolation);

signals:
  void animationDurationChanged(double animationDuration);
  void cameraVelocityChanged(double cameraVelocity);
  void pathLengthChanged(double pathLength);
  void pathChanged();

  void set_new_camera_frame(frame_t frame);

  void canPlayChanged(bool canPlay);

  void interpolationChanged(int interpolation);

protected:
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
  QVector<keypoint_t> _keypoints;
  QSharedPointer<const Interpolation> interpolation_implementation;

  double m_animationDuration = 10.;
  double m_cameraVelocity = 4.;
  double m_pathLength = 0.;

  bool m_canPlay = false;

  int m_interpolation = INTERPOLATION_LINEAR;

  QSharedPointer<const Interpolation> create_interpolation_implementation_for_enum(interpolation_t) const;

  void _init_connections();

private slots:
  void setPathLength(double pathLength);

  void newCameraPosition(double time);

  void updatePathLength();
  void updateCameraVelocits();
  void updateAnimationDuration();

  void setCanPlay(bool canPlay);
};

#endif // POINTCLOUDVIEWER_FLYTHROUGH_FLYTHROUGH_HPP_
