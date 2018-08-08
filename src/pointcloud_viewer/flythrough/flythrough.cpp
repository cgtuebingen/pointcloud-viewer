#include <pointcloud_viewer/flythrough/flythrough.hpp>
#include <pointcloud_viewer/flythrough/flythrough_file.hpp>

#include <QModelIndex>
#include <QMessageBox>

enum SECTION
{
  INDEX,
};

Flythrough::Flythrough(const Flythrough& flythrough)
  : _keypoints(flythrough._keypoints),
    interpolation_implementation(create_interpolation_implementation_for_enum(interpolation_t(flythrough.m_interpolation))),
    m_animationDuration(flythrough.m_animationDuration),
    m_cameraVelocity(flythrough.m_cameraVelocity),
    m_pathLength(flythrough.m_pathLength),
    m_canPlay(flythrough.m_canPlay),
    m_interpolation(flythrough.m_interpolation)
{
  _init_connections();
}

Flythrough::Flythrough()
{
  interpolation_implementation = QSharedPointer<Interpolation>(new LinearInterpolation(&this->_keypoints));

  _init_connections();
}

Flythrough::~Flythrough()
{
}

QSharedPointer<Flythrough> Flythrough::copy() const
{
  return QSharedPointer<Flythrough>(new Flythrough(*this));
}

void Flythrough::insert_keypoint(frame_t frame, int index)
{
  playback.stop();
  index = glm::clamp(index, 0, _keypoints.length());

  keypoint_t keypoint;

  keypoint.frame = frame;

  beginInsertRows(QModelIndex(), index, index+1);
  _keypoints.insert(index, keypoint);
  endInsertRows();
}

void Flythrough::delete_keypoint(int index)
{
  Q_ASSERT(index >= 0 && index < _keypoints.length());

  beginRemoveRows(QModelIndex(), index, index);
  _keypoints.removeAt(index);
  endRemoveRows();
}

void Flythrough::move_keypoint_up(int index)
{
  Q_ASSERT(index > 0 && index < _keypoints.length());

  if(!beginMoveRows(QModelIndex(), index, index,  QModelIndex(), index-1))
    return;
  std::swap(_keypoints[index], _keypoints[index-1]);
  endMoveRows();
}

void Flythrough::move_keypoint_down(int index)
{
  Q_ASSERT(index >= 0 && index < _keypoints.length()-1);

  if(!beginMoveRows(QModelIndex(), index, index,  QModelIndex(), index+2))
    return;
  std::swap(_keypoints[index], _keypoints[index+1]);
  endMoveRows();
}

keypoint_t Flythrough::keypoint_at(int index) const
{
  return _keypoints[index];
}

const QVector<keypoint_t>& Flythrough::all_keypoints() const
{
  return _keypoints;
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

  return interpolation_implementation->frame_for_overcome_distance(distance);
}

bool Flythrough::canPlay() const
{
  return m_canPlay;
}

int Flythrough::interpolation() const
{
  return m_interpolation;
}

void Flythrough::export_path(QString filepath) const
{
  try
  {
    FlythroughFile flythrough_file;

    flythrough_file.header.animation_duration = this->animationDuration();
    flythrough_file.header.camera_velocity = this->cameraVelocity();
    flythrough_file.header.interpolation = this->m_interpolation;

    flythrough_file.keypoint_frames.reserve(this->_keypoints.length());

    for(int i=0; i<_keypoints.length(); ++i)
      flythrough_file.keypoint_frames << _keypoints[i].frame;

    flythrough_file.export_to_file(filepath);

  }catch(QString message)
  {
    QMessageBox::warning(nullptr, "Couldn't export path", message);
  }
}

void Flythrough::import_path(QString filepath)
{
  beginResetModel();
  try
  {
    playback.stop();

    FlythroughFile flythrough_file = FlythroughFile::import_from_file(filepath);

    if(flythrough_file.header.interpolation >= INTERPOLATION_NUMBER_VALUES)
      throw QString("Invalid file!\n(Invalid interpolation type)");

    this->_keypoints.resize(flythrough_file.keypoint_frames.length());
    for(int i=0; i<_keypoints.length(); ++i)
      _keypoints[i].frame = flythrough_file.keypoint_frames[i];

    this->setCameraVelocity(flythrough_file.header.camera_velocity);
    this->setAnimationDuration(flythrough_file.header.animation_duration);

    this->updatePathLength();
    this->updateCanPlay();
  }catch(QString message)
  {
    QMessageBox::warning(nullptr, "Couldn't import path", message);
  }
  endResetModel();
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

  const double oldCameraVelocity = m_cameraVelocity;

  m_cameraVelocity = cameraVelocity;
  emit cameraVelocityChanged(m_cameraVelocity);

  playback._current_time *= oldCameraVelocity / this->cameraVelocity();
}

void Flythrough::setInterpolation(int interpolation)
{
  if (m_interpolation == interpolation)
    return;

  m_interpolation = interpolation;
  interpolation_implementation = create_interpolation_implementation_for_enum(interpolation_t(m_interpolation));
  emit interpolationChanged(m_interpolation);
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

QSharedPointer<const Interpolation> Flythrough::create_interpolation_implementation_for_enum(Flythrough::interpolation_t interpolation) const
{
  LinearInterpolation* implementation = nullptr;

  switch(interpolation)
  {
  case INTERPOLATION_LINEAR:
    implementation = new LinearInterpolation(&this->_keypoints, false);
    break;
  case INTERPOLATION_LINEAR_SMOOTHSTEP:
    implementation = new LinearInterpolation(&this->_keypoints, true);
    break;
  case INTERPOLATION_NUMBER_VALUES:
    Q_UNREACHABLE();
    implementation = new LinearInterpolation(&this->_keypoints, false);
    break;
  }

  Q_ASSERT(implementation != nullptr);

  return QSharedPointer<Interpolation>(implementation);
}

void Flythrough::_init_connections()
{
  connect(this, &Flythrough::animationDurationChanged, this, &Flythrough::updateCameraVelocits);
  connect(this, &Flythrough::cameraVelocityChanged, this, &Flythrough::updateAnimationDuration);
  connect(this, &Flythrough::pathLengthChanged, this, &Flythrough::updateAnimationDuration);

  connect(this, &Flythrough::rowsInserted, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::rowsMoved, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::rowsRemoved, this, &Flythrough::updatePathLength);
  connect(this, &Flythrough::dataChanged, this, &Flythrough::updatePathLength);

  connect(this, &Flythrough::rowsInserted, this, &Flythrough::updateCanPlay);
  connect(this, &Flythrough::rowsMoved, this, &Flythrough::updateCanPlay);
  connect(this, &Flythrough::rowsRemoved, this, &Flythrough::updateCanPlay);
  connect(this, &Flythrough::dataChanged, this, &Flythrough::updateCanPlay);

  connect(this, &Flythrough::pathLengthChanged, this, &Flythrough::pathChanged);
  connect(this, &Flythrough::rowsInserted, this, &Flythrough::pathChanged);
  connect(this, &Flythrough::rowsMoved, this, &Flythrough::pathChanged);
  connect(this, &Flythrough::rowsRemoved, this, &Flythrough::pathChanged);
  connect(this, &Flythrough::dataChanged, this, &Flythrough::pathChanged);

  playback._animationDuration = this->m_animationDuration;
  connect(this, &Flythrough::animationDurationChanged, this, [this](){playback._animationDuration = this->m_animationDuration;});
  connect(&playback, &Playback::request_next_frame, this, &Flythrough::newCameraPosition);

  updateCanPlay();
}

void Flythrough::setPathLength(double pathLength)
{
  pathLength = glm::max(0., pathLength);

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

void Flythrough::updateCanPlay()
{
  playback.only_one_frame = _keypoints.length() < 2;
  setCanPlay(_keypoints.isEmpty() == false);
}

void Flythrough::updatePathLength()
{
  setPathLength(interpolation_implementation->path_length());
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
