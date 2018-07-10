#ifndef POINTCLOUDVIEWER_NAVIGATION_HPP_
#define POINTCLOUDVIEWER_NAVIGATION_HPP_

#include <pointcloud_viewer/camera.hpp>

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

/*
Handles input events for implementing navigation
*/
class Navigation final : public QObject
{
  Q_OBJECT
public:
  Camera camera;
  glm::vec3 turntable_origin = glm::vec3(0);

  Navigation(QWidget* viewport);
  ~Navigation();

public slots:
  void startFpsNavigation();
  void stopFpsNavigation(bool keepNewFrame=true);

  void resetCameraLocation();
  void resetMovementSpeed();

  void wheelEvent(QWheelEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void focusOutEvent(QFocusEvent* event);

protected:
  void timerEvent(QTimerEvent* timerEvent);

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

  int _base_movement_speed = 0;
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
