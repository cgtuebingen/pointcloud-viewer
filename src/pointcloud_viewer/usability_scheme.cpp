#include <pointcloud_viewer/usability_scheme.hpp>

#include <QApplication>
#include <QSettings>

class UsabilityScheme::Implementation::BlenderScheme final : public UsabilityScheme::Implementation
{
public:
  enum mode_t
  {
    IDLE,
    FPS,
    TURNTABLE_ROTATE,
    TURNTABLE_SHIFT,
    TURNTABLE_ZOOM,
  };

  BlenderScheme(Navigation::Controller& navigation);

  void on_enable() override;
  void on_disable() override;

  void wheelEvent(QWheelEvent* event) override;
  void mouseMoveEvent(glm::vec2 mouse_force, QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void fps_mode_changed(bool enabled_fps_mode) override;
  QKeySequence fps_activation_key_sequence() override;

private:
  mode_t mode = IDLE;

  void enableMode(mode_t mode);
  void disableMode(mode_t mode);

  static glm::vec3 direction_for_key(QKeyEvent* event);
  static int speed_for_key(QKeyEvent* event);
};

class UsabilityScheme::Implementation::MeshLabScheme final : public UsabilityScheme::Implementation
{
public:
  MeshLabScheme(Navigation::Controller& navigation);

  void on_enable() override;
  void on_disable() override;

  void wheelEvent(QWheelEvent* event) override;
  void mouseMoveEvent(glm::vec2 mouse_force, QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void fps_mode_changed(bool enabled_fps_mode) override;
  QKeySequence fps_activation_key_sequence() override;
};


UsabilityScheme::UsabilityScheme(Navigation::Controller& navigation)
{
  implementations[BLENDER] = QSharedPointer<Implementation>(new Implementation::BlenderScheme(navigation));
  implementations[MESHLAB] = QSharedPointer<Implementation>(new Implementation::MeshLabScheme(navigation));

  QSettings settings;
  enableScheme(scheme_from_string(settings.value("Navigation/usabilityScheme", scheme_as_string(BLENDER)).toString()));
}

UsabilityScheme::~UsabilityScheme()
{
  if(_implementation != nullptr)
    _implementation->on_disable();

  QSettings settings;
  settings.setValue("Navigation/usabilityScheme", scheme_as_string(enabled_scheme()));
}

void UsabilityScheme::enableBlenderScheme()
{
  enableScheme(BLENDER);
}

void UsabilityScheme::enableMeshlabScheme()
{
  enableScheme(MESHLAB);
}

void UsabilityScheme::enableScheme(scheme_t scheme)
{
  if(_implementation != nullptr)
    _implementation->on_disable();

  _implementation = implementations.value(scheme, implementations.value(BLENDER)).data();

  if(_implementation != nullptr)
  {
    _implementation->on_enable();
    fpsActivationKeySequenceChanged(_implementation->fps_activation_key_sequence());
  }else
  {
    fpsActivationKeySequenceChanged(QKeySequence());
  }
}

UsabilityScheme::scheme_t UsabilityScheme::enabled_scheme() const
{
  for(auto scheme : implementations.values())
    if(scheme == _implementation)
      return implementations.key(scheme);
  return BLENDER;
}

void UsabilityScheme::wheelEvent(QWheelEvent* event)
{
  _implementation->wheelEvent(event);
}

void UsabilityScheme::mouseMoveEvent(glm::vec2 mouse_force, QMouseEvent* event)
{

  _implementation->mouseMoveEvent(mouse_force, event);
}

void UsabilityScheme::mousePressEvent(QMouseEvent* event)
{
  _implementation->mousePressEvent(event);
}

void UsabilityScheme::mouseReleaseEvent(QMouseEvent* event)
{
  _implementation->mouseReleaseEvent(event);
}

void UsabilityScheme::keyPressEvent(QKeyEvent* event)
{
  _implementation->keyPressEvent(event);
}

void UsabilityScheme::keyReleaseEvent(QKeyEvent* event)
{
  _implementation->keyReleaseEvent(event);
}

void UsabilityScheme::fps_mode_changed(bool enabled_fps_mode)
{
  _implementation->fps_mode_changed(enabled_fps_mode);
}

QKeySequence UsabilityScheme::fps_activation_key_sequence()
{
  return _implementation->fps_activation_key_sequence();
}

QString UsabilityScheme::scheme_as_string(UsabilityScheme::scheme_t scheme)
{
  switch(scheme)
  {
  case BLENDER:
    return "Blender";
  case MESHLAB:
    return "MeshLab";
  }
  return scheme_as_string(BLENDER);
}

UsabilityScheme::scheme_t UsabilityScheme::scheme_from_string(QString scheme)
{
  if(scheme == scheme_as_string(MESHLAB))
    return MESHLAB;
  else if(scheme == scheme_as_string(BLENDER))
    return BLENDER;
  else
    return BLENDER;
}

// ==== Implementation ====

UsabilityScheme::Implementation::Implementation(Navigation::Controller& navigation)
  : navigation(navigation)
{

}

// ==== Blender ====

UsabilityScheme::Implementation::BlenderScheme::BlenderScheme(Navigation::Controller& navigation)
  : Implementation(navigation)
{
}

void UsabilityScheme::Implementation::BlenderScheme::on_enable()
{

}

void UsabilityScheme::Implementation::BlenderScheme::on_disable()
{
  switch(mode)
  {
  case TURNTABLE_ROTATE:
  case TURNTABLE_SHIFT:
  case TURNTABLE_ZOOM:
    navigation.end_turntable();
    break;
  case FPS:
    navigation.stopFpsNavigation();
    break;
  case IDLE:
    break;
  }
}

void UsabilityScheme::Implementation::BlenderScheme::wheelEvent(QWheelEvent* event)
{
  if(mode == FPS)
  {
    if(event->modifiers() == Qt::NoModifier)
      navigation.incr_base_movement_speed(event->angleDelta().y() / 15);
    else if(event->modifiers() == Qt::CTRL)
      navigation.tilt_camera(event->angleDelta().y());
    else if(event->modifiers() == Qt::CTRL+Qt::SHIFT)
      navigation.tilt_camera(event->angleDelta().y() * 4.);
  }else
  {
    navigation.turntable_zoom(-event->angleDelta().y() / 120.f);
  }
}

void UsabilityScheme::Implementation::BlenderScheme::mouseMoveEvent(glm::vec2 mouse_force, QMouseEvent* event)
{
  Q_UNUSED(event);

  if(mode == IDLE || mode == FPS)
    return;

  switch(mode)
  {
  case TURNTABLE_ROTATE:
    navigation.turntable_rotate(mouse_force);
    break;
  case TURNTABLE_SHIFT:
    navigation.turntable_shift(mouse_force);
    break;
  case TURNTABLE_ZOOM:
    navigation.turntable_zoom(mouse_force.y);
    break;
  case FPS:
  case IDLE:
    break;
  }
}

void UsabilityScheme::Implementation::BlenderScheme::mousePressEvent(QMouseEvent* event)
{
  if(mode == FPS)
  {
    if(event->button() == Qt::LeftButton)
      navigation.stopFpsNavigation();
    if(event->button() == Qt::RightButton)
      navigation.stopFpsNavigation(false);
    if(event->button() == Qt::MiddleButton)
    {
      if(event->modifiers() == Qt::CTRL)
        navigation.reset_camera_tilt();
    }
  }

  if(mode == IDLE)
  {
    if(event->button() == Qt::MiddleButton)
    {
      navigation.begin_turntable();

      if(event->modifiers() == Qt::NoModifier)
        enableMode(TURNTABLE_ROTATE);
      else if(event->modifiers() == Qt::ShiftModifier)
        enableMode(TURNTABLE_SHIFT);
      else if(event->modifiers() == Qt::ControlModifier)
        enableMode(TURNTABLE_ZOOM);
    }else if(event->button() == Qt::LeftButton)
    {
      if(event->modifiers() == Qt::NoModifier)
      {
        const glm::ivec2 screenspace_pixel = glm::ivec2(event->x(), event->y());

        navigation.pick_point(screenspace_pixel);
      }
    }
  }
}

void UsabilityScheme::Implementation::BlenderScheme::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::MiddleButton)
  {
    disableMode(TURNTABLE_ROTATE);
    disableMode(TURNTABLE_SHIFT);
    disableMode(TURNTABLE_ZOOM);
  }
}

void UsabilityScheme::Implementation::BlenderScheme::keyPressEvent(QKeyEvent* event)
{
  if(mode == FPS)
  {
    if(event->modifiers() == Qt::NoModifier)
    {
      if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        navigation.stopFpsNavigation();
      if(event->key() == Qt::Key_Escape)
        navigation.stopFpsNavigation(false);
    }

    if(event->modifiers() == Qt::AltModifier)
    {
      if(event->key() == Qt::Key_F4)
      {
        navigation.stopFpsNavigation();
        QApplication::quit();
        return;
      }
    }

    navigation.key_direction += direction_for_key(event);
    navigation.key_speed += speed_for_key(event);
    navigation.update_key_force();
  }
}

void UsabilityScheme::Implementation::BlenderScheme::keyReleaseEvent(QKeyEvent* event)
{
  if(mode == FPS)
  {
    navigation.key_direction -= direction_for_key(event);
    navigation.key_speed -= speed_for_key(event);
    navigation.update_key_force();
  }
}

void UsabilityScheme::Implementation::BlenderScheme::fps_mode_changed(bool enabled_fps_mode)
{
  if(enabled_fps_mode)
    enableMode(FPS);
  else
    disableMode(FPS);
}

QKeySequence UsabilityScheme::Implementation::BlenderScheme::fps_activation_key_sequence()
{
  return QKeySequence(Qt::SHIFT + Qt::Key_F);
}

void UsabilityScheme::Implementation::BlenderScheme::enableMode(mode_t mode)
{
  if(this->mode == IDLE)
  {
    this->mode = mode;
  }
}

void UsabilityScheme::Implementation::BlenderScheme::disableMode(mode_t mode)
{
  if(this->mode == mode)
  {
    switch(this->mode)
    {
    case TURNTABLE_ZOOM:
    case TURNTABLE_SHIFT:
    case TURNTABLE_ROTATE:
      navigation.end_turntable();
      break;
    case IDLE:
    case FPS:
      break;
    }

    this->mode = IDLE;
  }
}

glm::vec3 UsabilityScheme::Implementation::BlenderScheme::direction_for_key(QKeyEvent* event)
{
  glm::vec3 key_direction;
  if(event->key() == Qt::Key_W)
    key_direction.y += 1.f;
  if(event->key() == Qt::Key_Up)
    key_direction.y += 1.f;
  if(event->key() == Qt::Key_S)
    key_direction.y -= 1.f;
  if(event->key() == Qt::Key_Down)
    key_direction.y -= 1.f;
  if(event->key() == Qt::Key_A)
    key_direction.x -= 1.f;
  if(event->key() == Qt::Key_Left)
    key_direction.x -= 1.f;
  if(event->key() == Qt::Key_D)
    key_direction.x += 1.f;
  if(event->key() == Qt::Key_Right)
    key_direction.x += 1.f;
  if(event->key() == Qt::Key_E)
    key_direction.z += 1.f;
  if(event->key() == Qt::Key_Q)
    key_direction.z -= 1.f;
  return key_direction;
}

int UsabilityScheme::Implementation::BlenderScheme::speed_for_key(QKeyEvent* event)
{
  int key_speed = 0;
  if(event->key() == Qt::Key_Shift)
    key_speed++;
  return key_speed;
}

// ==== Meshlab ====

UsabilityScheme::Implementation::MeshLabScheme::MeshLabScheme(Navigation::Controller& navigation)
  : Implementation(navigation)
{
}

void UsabilityScheme::Implementation::MeshLabScheme::on_enable()
{

}

void UsabilityScheme::Implementation::MeshLabScheme::on_disable()
{

}

void UsabilityScheme::Implementation::MeshLabScheme::wheelEvent(QWheelEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mouseMoveEvent(glm::vec2 mouse_force, QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mousePressEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mouseReleaseEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::keyPressEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::keyReleaseEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::fps_mode_changed(bool enabled_fps_mode)
{

}

QKeySequence UsabilityScheme::Implementation::MeshLabScheme::fps_activation_key_sequence()
{
  return QKeySequence();
}