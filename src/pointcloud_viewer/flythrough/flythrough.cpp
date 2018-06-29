#include <pointcloud_viewer/flythrough/flythrough.hpp>

#include <QModelIndex>

enum SECTION
{
  INDEX,
};

Flythrough::Flythrough()
{
  connect(this, &Flythrough::animationDurationChanged, this, &Flythrough::updateCameraVelocits);
  connect(this, &Flythrough::cameraVelocityChanged, this, &Flythrough::updateAnimationDuration);
  connect(this, &Flythrough::pathLengthChanged, this, &Flythrough::updateAnimationDuration);

  connect(this, &Flythrough::dataChanged, this, &Flythrough::updatePathLength);
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
  cameraVelocity = glm::clamp(cameraVelocity, 1.e-5, 1.e10);

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

  if(qFuzzyCompare(m_pathLength, pathLength))
    return;

  m_pathLength = pathLength;
  emit pathLengthChanged(m_pathLength);
}

void Flythrough::updatePathLength()
{
  double pathLength = 0;
  for(int i=1; i<_keypoints.length(); ++i)
    pathLength += double(glm::distance<float>(_keypoints[i].frame.position, _keypoints[i-1].frame.position));

  setPathLength(pathLength);
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
