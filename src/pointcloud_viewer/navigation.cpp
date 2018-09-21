#include <pointcloud_viewer/navigation.hpp>
#include <pointcloud_viewer/camera.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud_viewer/usability_scheme.hpp>
#include <geometry/plane.hpp>
#include <core_library/print.hpp>

#include <glm/gtx/io.hpp>

#include <QWidget>
#include <QApplication>
#include <QSettings>

Navigation::Navigation(Viewport* viewport)
  : viewport(viewport),
    _controller(new Controller(*this)),
    _usability_scheme(new UsabilityScheme(*_controller))
{
  connect(viewport, &Viewport::frame_rendered, this, &Navigation::updateFrameRenderDuration);

  _turntable_origin_relative_to_camera = Camera().frame.inverse() * glm::vec3(0);

  QSettings settings;
  _mouse_sensitivity_value = settings.value("Navigation/mouseSensitivity", 0).value<int>();
}

Navigation::~Navigation()
{
  _controller->stopFpsNavigation();

  QSettings settings;
  settings.setValue("Navigation/mouseSensitivity", int(_mouse_sensitivity_value));

  delete _usability_scheme;
  delete _controller;
}

UsabilityScheme& Navigation::usabilityScheme()
{
  return *_usability_scheme;
}

void Navigation::startFpsNavigation()
{
  if(!fps_mode)
  {
    fps_mode = true;
    fps_start_frame = camera.frame;
    fps_timer = startTimer(40);
    num_frames_in_fps_mode = 0;
    _usability_scheme->fps_mode_changed(true);
    viewport->grabMouse(Qt::BlankCursor);
    viewport->grabKeyboard();
    viewport->setMouseTracking(true);
  }
}

void Navigation::stopFpsNavigation(bool keepNewFrame)
{
  if(fps_mode)
  {
    fps_mode = false;
    if(!keepNewFrame)
    {
      camera.frame = fps_start_frame;
      viewport->update();
    }

    killTimer(fps_timer);
    fps_timer = 0;
    _usability_scheme->fps_mode_changed(false);
    viewport->releaseKeyboard();
    viewport->releaseMouse();
    viewport->setMouseTracking(false);
  }
}

void Navigation::resetCameraLocation()
{
  camera.frame = Camera().frame;

  turntable_origin = glm::vec3(0);
  _turntable_origin_relative_to_camera = Camera().frame.inverse() * glm::vec3(0);

  viewport->update();
}

void Navigation::resetMovementSpeed()
{
  _base_movement_speed = 0;
}

void Navigation::updateFrameRenderDuration(double duration)
{
  // 0.04 because the timer limits the minimal time between to events to be 40ms anyway
  _last_frame_duration = glm::clamp(float(duration), 0.04f, 0.1f);
}

void Navigation::wheelEvent(QWheelEvent* event)
{
  _usability_scheme->wheelEvent(event);
}

void Navigation::mouseMoveEvent(QMouseEvent* event)
{
  glm::vec2 mouse_force = glm::vec2(0.f);

  const glm::ivec2 current_mouse_pos(event->x(), event->y());

  bool handle_event = true;

  if(fps_mode)
  {
    const glm::ivec2 center = viewport_center();

    if(distance(current_mouse_pos - center, center) == VERY_FAR)
      this->set_mouse_pos(center);

    if(distance(current_mouse_pos - last_mouse_pos, center) != CLOSE)
      handle_event = false;

    if(event->source() != Qt::MouseEventNotSynthesized)
      handle_event = false;
  }

  if(handle_event)
  {
    mouse_force = glm::vec2(current_mouse_pos - last_mouse_pos) * 0.4f * mouse_sensitivity() * _last_frame_duration;

    mouse_force = glm::clamp(glm::vec2(-20), glm::vec2(20), mouse_force);

    if(fps_mode)
      fps_rotation(mouse_force);
    else
      _usability_scheme->mouseMoveEvent(mouse_force, event);
  }

  last_mouse_pos = current_mouse_pos;

  event->accept();
}

void Navigation::mousePressEvent(QMouseEvent* event)
{
  last_mouse_pos = glm::ivec2(event->x(), event->y());
  _usability_scheme->mousePressEvent(event);
}

void Navigation::mouseReleaseEvent(QMouseEvent* event)
{
  last_mouse_pos = glm::ivec2(event->x(), event->y());
  _usability_scheme->mouseReleaseEvent(event);
}

void Navigation::keyPressEvent(QKeyEvent* event)
{
  _usability_scheme->keyPressEvent(event);
}

void Navigation::keyReleaseEvent(QKeyEvent* event)
{
  _usability_scheme->keyReleaseEvent(event);
}

void Navigation::focusOutEvent(QFocusEvent* event)
{
  Q_UNUSED(event);
  _controller->stopFpsNavigation();
}

glm::ivec2 Navigation::mouse_sensitivity_value_range() const
{
  return glm::ivec2(-100, 100);
}

int Navigation::mouse_sensitivity_value() const
{
  return _mouse_sensitivity_value;
}

void Navigation::set_mouse_sensitivity_value(int value)
{
  if(_mouse_sensitivity_value == value)
    return;

  _mouse_sensitivity_value = glm::clamp<int>(value, mouse_sensitivity_value_range()[0], mouse_sensitivity_value_range()[1]);

  mouse_sensitivity_value_changed(value);
}

void Navigation::handle_new_point_cloud()
{
  setTurntableOrigin(find_best_turntable_origin());
}

void Navigation::timerEvent(QTimerEvent* timerEvent)
{
  if(timerEvent->timerId() != fps_timer || !fps_mode)
    return;

  navigate_fps();

  viewport->update();

  ++num_frames_in_fps_mode;
}

glm::ivec2 Navigation::viewport_center() const
{
  QSize size = viewport->size();

  return glm::ivec2(size.width()/2,size.height()/2);
}

Navigation::distance_t Navigation::distance(glm::ivec2 difference, glm::ivec2 radius) const
{
  difference = glm::abs(difference);

  auto is_very_far = [](int value, int radius) {
    return value > radius*8/10;
  };

  auto is_far = [](int value, int radius) {
    return value > radius/2;
  };

  if(is_very_far(difference.x, radius.x) || is_very_far(difference.y, radius.y))
    return VERY_FAR;
  else if(is_far(difference.x, radius.x) || is_far(difference.y, radius.y))
    return FAR;
  else
    return CLOSE;
}

void Navigation::tilt_camera(double factor)
{
  const glm::vec3 forward = camera.frame.orientation * glm::vec3(0, 0, -1);
  const float angle = float(factor * 0.1 / (120. * glm::pi<double>()));

  glm::quat rotation = glm::angleAxis(angle, forward);

  camera.frame.orientation = rotation * camera.frame.orientation;
  viewport->update();
}

void Navigation::reset_camera_tilt()
{
  camera.frame = remove_tilt(camera.frame);
  viewport->update();
}

void Navigation::incr_base_movement_speed(int incr)
{
  _base_movement_speed = glm::clamp(incr+_base_movement_speed, -480, 320);
}

float Navigation::mouse_sensitivity() const
{
  return glm::pow(1.03f, float(_mouse_sensitivity_value));
}

glm::vec3 Navigation::find_best_turntable_origin()
{
  aabb_t aabb = viewport->aabb();

  glm::vec3 v = camera.frame * _turntable_origin_relative_to_camera;

  if(aabb.is_valid())
    v = glm::clamp(v, aabb.min_point, aabb.max_point);

  return v;
}

void Navigation::setTurntableOrigin(glm::vec3 origin)
{
  turntable_origin = origin;

  viewport->visualization().set_turntable_origin(turntable_origin);
  viewport->update();
}

float Navigation::base_movement_speed() const
{
  return glm::pow(1.01f, float(_base_movement_speed));
}

void Navigation::navigate_fps()
{
  if(!fps_mode)
    return;

  frame_t& view = camera.frame;

  const glm::vec3 forward = view.orientation * glm::vec3(0, 0, -1);
  const glm::vec3 up = view.orientation * glm::vec3(0, 1, 0);
  const glm::vec3 right = view.orientation * glm::vec3(1, 0, 0);

  const glm::vec3 key_force = _controller->key_force;

  const glm::vec3 movement =  up * key_force.z + forward * key_force.y + right * key_force.x;

  view.position += movement * base_movement_speed();
//  turntable_origin += movement;
}

void Navigation::fps_rotation(glm::vec2 mouse_force)
{
  frame_t& view = camera.frame;

  const glm::vec3 right = view.orientation * glm::vec3(1, 0, 0);

  view.orientation = glm::angleAxis(-mouse_force.x, glm::vec3(0,0,1)) * glm::angleAxis(-mouse_force.y, right) * view.orientation;
}

void Navigation::set_mouse_pos(glm::ivec2 mouse_pos)
{
  QCursor cursor = viewport->cursor();
  cursor.setPos(viewport->mapToGlobal(QPoint(mouse_pos.x, mouse_pos.y)));
  viewport->setCursor(cursor);
}

void Navigation::Controller::pick_point(const glm::ivec2 screenspace_pixel)
{
  navigation.simpleLeftClick(screenspace_pixel);
}

void Navigation::Controller::incr_base_movement_speed(int incr)
{
  navigation.incr_base_movement_speed(incr);
}

void Navigation::Controller::tilt_camera(double factor)
{
  navigation.tilt_camera(factor);
}

void Navigation::Controller::reset_camera_tilt()
{
  navigation.reset_camera_tilt();
}

void Navigation::Controller::startFpsNavigation()
{
  key_direction = glm::vec3(0);
  key_speed = 0;
  key_force = glm::vec3(0);
  navigation.startFpsNavigation();
}

void Navigation::Controller::stopFpsNavigation(bool keepNewFrame)
{
  key_direction = glm::vec3(0);
  key_speed = 0;
  key_force = glm::vec3(0);
  navigation.stopFpsNavigation(keepNewFrame);
}

void Navigation::Controller::show_trackball()
{
  const ray_t camera_center_ray = camera.ray_for_clipspace_point(glm::vec2(0));
  const ray_t camera_upper_half_ray = camera.ray_for_clipspace_point(glm::vec2(0, 0.5f));
  glm::vec3 trackball_center = camera_center_ray.get_point(Camera().frame.position.length());
  plane_t origin_plane = plane_t::from_normal(camera_center_ray.direction, trackball_center);
  float trackball_radius = glm::distance(trackball_center, camera_upper_half_ray.get_point(origin_plane.intersection_distance(camera_upper_half_ray)));

  navigation.viewport->visualization().settings.enable_trackball = true;
  navigation.viewport->visualization().set_trackball(trackball_center, trackball_radius);
  navigation.trackball_center = trackball_center;
  navigation.trackball_radius = trackball_radius;
  navigation.viewport->update();
}

void Navigation::Controller::begin_trackball_action()
{
}

void Navigation::Controller::trackball_rotate(glm::vec2 mouse_force, glm::ivec2 screenspace_pixel)
{
  // TODO: incooperate the screenspace pixel
  _rotate(navigation.trackball_center, mouse_force, up_vector(), right_vector());
}

void Navigation::Controller::trackball_shift(glm::vec2 mouse_force)
{
  navigation.trackball_center += _shift(mouse_force);
  navigation.viewport->visualization().set_trackball(navigation.trackball_center, navigation.trackball_radius);
}

void Navigation::Controller::trackball_zoom(float mouse_force_y)
{
  // TODO zooms to the wrong pixel
  _zoom(navigation.trackball_center, mouse_force_y);
}

void Navigation::Controller::end_trackball_action()
{
}

void Navigation::Controller::hide_trackball()
{
  navigation.viewport->visualization().settings.enable_trackball = false;
  navigation.viewport->update();
}

void Navigation::Controller::show_grid()
{
  navigation.viewport->visualization().settings.enable_grid = true;
  navigation.viewport->update();
}

void Navigation::Controller::hide_grid()
{
  navigation.viewport->visualization().settings.enable_grid = false;
  navigation.viewport->update();
}

void Navigation::Controller::begin_turntable_action()
{
  navigation.setTurntableOrigin(navigation.find_best_turntable_origin());
}

void Navigation::Controller::end_turntable_action()
{
  navigation.setTurntableOrigin(navigation.find_best_turntable_origin());
}

void Navigation::Controller::turntable_rotate(glm::vec2 mouse_force)
{
  _rotate(navigation.turntable_origin, mouse_force, glm::vec3(0,0,1), right_vector());
}

void Navigation::Controller::turntable_shift(glm::vec2 mouse_force)
{
  navigation.turntable_origin += _shift(mouse_force);
}

void Navigation::Controller::turntable_zoom(float mouse_force_y)
{
  const glm::vec3 turntable_origin = navigation.turntable_origin;

  _zoom(turntable_origin, mouse_force_y);

  navigation._turntable_origin_relative_to_camera = camera.frame.inverse() * turntable_origin;
  navigation.viewport->update();
}

glm::vec3 Navigation::Controller::forward_vector() const
{
  const glm::vec3 forward = camera.frame.orientation * glm::vec3(0, 0, -1);

  return forward;
}

glm::vec3 Navigation::Controller::up_vector() const
{
  const glm::vec3 up = camera.frame.orientation * glm::vec3(0, 1, 0);

  return up;
}

glm::vec3 Navigation::Controller::right_vector() const
{
  const glm::vec3 right = camera.frame.orientation * glm::vec3(1, 0, 0);

  return right;
}

void Navigation::Controller::update_key_force()
{
  if(glm::length(key_direction) > 0.5f)
    key_force = glm::normalize(key_direction) * glm::exp2(glm::clamp<float>(-1, 1, key_speed)) * 0.5f;
  else
    key_force = glm::vec3(0.f);
}

Navigation::Controller::Controller(Navigation& navigation)
  : camera(navigation.camera),
    navigation(navigation)
{
}

void Navigation::Controller::_rotate(glm::vec3 rotation_origin, glm::vec2 mouse_force, glm::vec3 x_rotation_axis, glm::vec3 y_rotation_axis)
{
  const float factor = 0.5f;
  frame_t& view = navigation.camera.frame;

  view.position -= rotation_origin;
  view = frame_t(rotation_origin, glm::angleAxis(factor * -mouse_force.x, x_rotation_axis) * glm::angleAxis(factor * -mouse_force.y, y_rotation_axis)) * view;
  navigation.viewport->update();
}

glm::vec3 Navigation::Controller::_shift(glm::vec2 mouse_force)
{
  const float factor = 0.5f;
  frame_t& view = navigation.camera.frame;

  glm::vec3 shift = up_vector() * mouse_force.y - right_vector() * mouse_force.x;
  shift *= factor;
  view.position += shift;
  navigation.viewport->update();
  return shift;
}

void Navigation::Controller::_zoom(glm::vec3 origin, float mouse_force_y)
{
  const float factor = 0.5f;
  frame_t& view = navigation.camera.frame;
  const glm::vec3 previous_zoom = view.position - origin;

  float zoom_factor = glm::clamp(0.5f, 1.5f, glm::exp2(factor * mouse_force_y));

  if(zoom_factor * length(previous_zoom) > 1.e-2f)
    view.position = origin + zoom_factor * previous_zoom;
  navigation.viewport->update();
}
