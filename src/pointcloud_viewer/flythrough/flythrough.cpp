#include <pointcloud_viewer/flythrough/flythrough.hpp>

#include <QModelIndex>

enum SECTION
{
  INDEX,
};

Flythrough::Flythrough()
{
  interpolation = new LinearInterpolation(&this->_keypoints);

  connect(this, &Flythrough::animationDurationChanged, this, &Flythrough::updateCameraVelocits);
  connect(this, &Flythrough::cameraVelocityChanged, this, &Flythrough::updateAnimationDuration);
  connect(this, &Flythrough::pathLengthChanged, this, &Flythrough::updateAnimationDuration);

  connect(this, &Flythrough::rowsInserted, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::rowsMoved, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::rowsRemoved, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::dataChanged, this, &Flythrough::updatePathLength);

  playback._animationDuration = this->m_animationDuration;
  connect(this, &Flythrough::animationDurationChanged, this, [this](){playback._animationDuration = this->m_animationDuration;});
  connect(&playback, &Playback::request_next_frame, this, &Flythrough::newCameraPosition);
}

Flythrough::~Flythrough()
{
  delete interpolation;
}

void Flythrough::insert_keypoint(frame_t frame, int index)
{
  index = glm::clamp(index, 0, _keypoints.length());

  keypoint_t keypoint;

  keypoint.frame = frame;

  beginInsertRows(QModelIndex(), index, index+1);
  _keypoints.insert(index, keypoint);
  endInsertRows();
}

keypoint_t Flythrough::keypoint_at(int index) const
{
  return _keypoints[index];
}

double Flythrough::animationDuration() const
{
  return m_animationDuration;
}

double Flythrough::cameraVelocity() const
{
  return m_cameraVelocity;
}

double Flythrough::pathLength() const
{
  return m_pathLength;
}

frame_t Flythrough::camera_position_for_time(double time, frame_t fallback) const
{
  if(_keypoints.isEmpty())
    return fallback;

  if(time < 0.)
    return _keypoints.first().frame;
  if(time > animationDuration())
    return _keypoints.last().frame;

  double distance = cameraVelocity() * time;

  return interpolation->frame_for_overcome_distance(distance);
}

bool Flythrough::canPlay() const
{
  return m_canPlay;
}

void Flythrough::setAnimationDuration(double animationDuration)
{
  animationDuration = glm::max(1.e-2, animationDuration);

  m_animationDuration = animationDuration;
  emit animationDurationChanged(animationDuration);

  if(qFuzzyCompare(m_animationDuration, animationDuration))
    return;

  m_animationDuration = animationDuration;
  emit animationDurationChanged(m_animationDuration);
}

void Flythrough::setCameraVelocity(double cameraVelocity)
{
  cameraVelocity = glm::clamp(cameraVelocity, 1.e-2, 1.e10);

  if(qFuzzyCompare(m_cameraVelocity, cameraVelocity))
    return;

  m_cameraVelocity = cameraVelocity;
  emit cameraVelocityChanged(m_cameraVelocity);
}

int Flythrough::rowCount(const QModelIndex& parent) const
{
  if(parent.isValid())
    return 0;

  return _keypoints.length();
}

QVariant Flythrough::data(const QModelIndex& index, int role) const
{
  switch(role)
  {
  case Qt::DisplayRole:
    switch(SECTION(index.column()))
    {
    case SECTION::INDEX:
      return index.row();
    }
    return QVariant();
  default:
    return QVariant();
  }
}

QVariant Flythrough::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);

  switch(role)
  {
  case Qt::DisplayRole:
    switch(SECTION(section))
    {
    case SECTION::INDEX:
      return "#";
    }
    return QVariant();
  default:
    return QVariant();
  }
}

void Flythrough::setPathLength(double pathLength)
{
  pathLength = glm::max(0., pathLength);

  setCanPlay(pathLength > 0.);

  if(qFuzzyCompare(m_pathLength, pathLength))
    return;

  m_pathLength = pathLength;
  emit pathLengthChanged(m_pathLength);
}

void Flythrough::newCameraPosition(double time)
{
  if(Q_UNLIKELY(!canPlay()))
  {
    playback.stop();
    return;
  }

  const frame_t invalid(glm::vec3(0), quat_identity<glm::quat>(), -1.f);

  frame_t new_frame = camera_position_for_time(time, invalid);

  if(Q_LIKELY(new_frame.scale_factor > 0.f))
    set_new_camera_frame(new_frame);
}

void Flythrough::updatePathLength()
{
  setPathLength(interpolation->path_length());
}

void Flythrough::updateCameraVelocits()
{
  if(pathLength() > 1)
    setCameraVelocity(pathLength() / animationDuration());
}

void Flythrough::updateAnimationDuration()
{
  if(pathLength() > 1)
    setAnimationDuration(pathLength() / cameraVelocity());
}

void Flythrough::setCanPlay(bool canPlay)
{
  if (m_canPlay == canPlay)
    return;

  m_canPlay = canPlay;
  emit canPlayChanged(m_canPlay);
}
