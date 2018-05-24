#ifndef POINTCLOUDVIEWER_NAVIGATION_HPP_
#define POINTCLOUDVIEWER_NAVIGATION_HPP_

#include <pointcloud_viewer/camera.hpp>

#include <QObject>
#include <QMouseEvent>
#include <QKeyEvent>

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
  void stopFpsNavigation();

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

  QWidget* const viewport;

  mode_t mode = IDLE;

  glm::vec3 key_direction = glm::vec3(0.f);
  int key_speed = 0;
  glm::vec3 key_force = glm::vec3(0.f);
  glm::vec2 mouse_force = glm::vec2(0.f);

  glm::ivec2 last_mouse_pos;

  int fps_timer = 0;
  int num_frames_in_fps_mode = 0;

  void update_key_force();

  void navigate();

  void enableMode(mode_t mode);
  void disableMode(mode_t mode);
};


#endif // POINTCLOUDVIEWER_NAVIGATION_HPP_
