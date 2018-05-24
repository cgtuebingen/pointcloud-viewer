#include <pointcloud_viewer/navigation.hpp>

#include <glm/gtx/io.hpp>

#include <QWidget>
#include <QApplication>

Navigation::Navigation(QWidget* viewport)
  : viewport(viewport)
{

}

Navigation::~Navigation()
{
  stopFpsNavigation();
}

void Navigation::startFpsNavigation()
{
  if(mode == Navigation::IDLE)
  {
    fps_timer = startTimer(40);
    key_direction = glm::vec3(0);
    key_speed = 0;
    num_frames_in_fps_mode = 0;
    enableMode(Navigation::FPS);
    viewport->grabMouse(Qt::BlankCursor);
    viewport->grabKeyboard();
    viewport->setMouseTracking(true);
  }
}

void Navigation::stopFpsNavigation()
{
  if(mode == Navigation::FPS)
  {
    killTimer(fps_timer);
    fps_timer = 0;
    key_direction = glm::vec3(0);
    key_speed = 0;
    disableMode(Navigation::FPS);
    viewport->releaseKeyboard();
    viewport->releaseMouse();
    viewport->setMouseTracking(false);
  }
}

void Navigation::mouseMoveEvent(QMouseEvent* event)
{
  const glm::ivec2 current_mouse_pos(event->x(), event->y());
  mouse_force = glm::vec2(current_mouse_pos - last_mouse_pos) * 0.01f;

  mouse_force = glm::clamp(glm::vec2(-20), glm::vec2(20), mouse_force);

  if(mode == Navigation::TURNTABLE_ROTATE || mode == Navigation::TURNTABLE_SHIFT || mode == Navigation::TURNTABLE_ZOOM)
  {
    navigate();
    viewport->update();
  }

  last_mouse_pos = current_mouse_pos;

}

void Navigation::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::MiddleButton)
  {
    last_mouse_pos = glm::ivec2(event->x(), event->y());

    if(event->modifiers() == Qt::NoModifier)
      enableMode(Navigation::TURNTABLE_ROTATE);
    else if(event->modifiers() == Qt::ShiftModifier)
      enableMode(Navigation::TURNTABLE_SHIFT);
    else if(event->modifiers() == Qt::ControlModifier)
      enableMode(Navigation::TURNTABLE_ZOOM);
  }
}

void Navigation::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::MiddleButton)
  {
    disableMode(Navigation::TURNTABLE_ROTATE);
    disableMode(Navigation::TURNTABLE_SHIFT);
    disableMode(Navigation::TURNTABLE_ZOOM);
  }
}

inline glm::vec3 direction_for_key(QKeyEvent* event)
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
  return key_direction;
}

inline int speed_for_key(QKeyEvent* event)
{
  int key_speed = 0;
  if(event->key() == Qt::Key_Shift)
    key_speed++;
  return key_speed;
}

void Navigation::keyPressEvent(QKeyEvent* event)
{
  if(mode == FPS)
  {
    if(event->key() == Qt::Key_Escape && event->modifiers() == Qt::NoModifier)
      stopFpsNavigation();
    if(event->key() == Qt::Key_F4 && event->modifiers() == Qt::AltModifier)
    {
      stopFpsNavigation();
      QApplication::quit();
    }

    key_direction += direction_for_key(event);
    key_speed += speed_for_key(event);
    update_key_force();
  }
}

void Navigation::keyReleaseEvent(QKeyEvent* event)
{
  key_direction -= direction_for_key(event);
  key_speed -= speed_for_key(event);
  update_key_force();
}

void Navigation::focusOutEvent(QFocusEvent* event)
{
  Q_UNUSED(event);

  stopFpsNavigation();
}

void Navigation::timerEvent(QTimerEvent* timerEvent)
{
  if(timerEvent->timerId() != fps_timer || mode!=FPS)
    return;

  if(num_frames_in_fps_mode == 0)
    mouse_force = glm::vec2(0);

  navigate();

  viewport->update();

  ++num_frames_in_fps_mode;
}

void Navigation::update_key_force()
{
  if(glm::length(key_direction) > 0.5f)
    key_force = glm::normalize(key_direction) * glm::exp2(glm::clamp<float>(-1, 1, key_speed)) * 0.5f;
  else
    key_force = glm::vec3(0.f);
}

void Navigation::navigate()
{
  if(mode == IDLE)
    return;

  frame_t& view = camera.frame;

  const glm::vec3 forward = view.orientation * glm::vec3(0, 0, -1);
  const glm::vec3 up = view.orientation * glm::vec3(0, 1, 0);
  const glm::vec3 right = view.orientation * glm::vec3(1, 0, 0);

  switch(mode)
  {
  case FPS:
  {
    const glm::vec3 movement = forward * key_force.y + right * key_force.x;

    view.orientation = glm::angleAxis(-mouse_force.x, glm::vec3(0,0,1)) * glm::angleAxis(-mouse_force.y, right) * view.orientation;

    view.position += movement;
    turntable_origin += movement;
    break;
  }
  case TURNTABLE_ROTATE:
  {
    view.position -= turntable_origin;
    view = frame_t(turntable_origin, glm::angleAxis(-mouse_force.x, glm::vec3(0,0,1)) * glm::angleAxis(-mouse_force.y, right)) * view;
    break;
  }
  case TURNTABLE_SHIFT:
  {
    const glm::vec3 shift = up * mouse_force.y - right * mouse_force.x;
    view.position += shift;
    turntable_origin += shift;
    break;
  }
  case TURNTABLE_ZOOM:
  {
    glm::vec3 previous_zoom = view.position - turntable_origin;

    float zoom_factor = glm::clamp(0.5f, 1.5f, glm::exp2(mouse_force.y));

    if(zoom_factor * length(previous_zoom) > 1.e-2f)
      view.position = turntable_origin + zoom_factor * previous_zoom;
    break;
  }
  case IDLE:
    break;
  }

  mouse_force = glm::vec2(0);
}

void Navigation::enableMode(Navigation::mode_t mode)
{
  if(this->mode == IDLE)
    this->mode = mode;
}

void Navigation::disableMode(Navigation::mode_t mode)
{
  if(this->mode == mode)
    this->mode = IDLE;
}
