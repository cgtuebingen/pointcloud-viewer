#ifndef POINTCLOUDVIEWER_NAVIGATION_HPP_
#define POINTCLOUDVIEWER_NAVIGATION_HPP_

#include <pointcloud_viewer/camera.hpp>

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

class Viewport;

/*
Handles input events for implementing navigation
*/
class Navigation final : public QObject
{
  Q_OBJECT
public:
  Camera camera;
  glm::vec3 turntable_origin = glm::vec3(0);

  Navigation(Viewport* viewport);
  ~Navigation();

public slots:
  void startFpsNavigation();
  void stopFpsNavigation(bool keepNewFrame=true);

  void resetCameraLocation();
  void resetMovementSpeed();

  void updateFrameRenderDuration(double duration);

  void wheelEvent(QWheelEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void focusOutEvent(QFocusEvent* event);

  glm::ivec2 mouse_sensitivity_value_range() const;
  int mouse_sensitivity_value() const;
  void set_mouse_sensitivity_value(int value);

protected:
  void timerEvent(QTimerEvent* timerEvent);

signals:
  void mouse_sensitivity_value_changed(int value);

private:
  enum mode_t
  {
    IDLE,
    FPS,
    TURNTABLE_ROTATE,
    TURNTABLE_SHIFT,
    TURNTABLE_ZOOM,
  };

  enum distance_t
  {
    CLOSE,
    FAR,
    VERY_FAR,
  };

  QWidget* const viewport;

  mode_t mode = IDLE;


  glm::vec3 key_direction = glm::vec3(0.f);
  int key_speed = 0;
  glm::vec3 key_force = glm::vec3(0.f);
  glm::vec2 mouse_force = glm::vec2(0.f);

  glm::ivec2 last_mouse_pos;

  glm::ivec2 viewport_center() const;
  distance_t distance(glm::ivec2 difference, glm::ivec2 radius) const;

  void incr_base_movement_speed(int incr);
  float base_movement_speed() const;
  float mouse_sensitivity() const;

  float _last_frame_duration = 1/60.f;

  int _base_movement_speed = 0;
  int _mouse_sensitivity_value = 0;
  frame_t fps_start_frame;
  int fps_timer = 0;
  int num_frames_in_fps_mode = 0;

  void update_key_force();

  void navigate();

  void enableMode(mode_t mode);
  void disableMode(mode_t mode);

  void set_mouse_pos(glm::ivec2 mouse_pos);
};


#endif // POINTCLOUDVIEWER_NAVIGATION_HPP_
