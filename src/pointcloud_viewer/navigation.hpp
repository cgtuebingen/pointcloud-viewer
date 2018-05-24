#ifndef POINTCLOUDVIEWER_NAVIGATION_HPP_
#define POINTCLOUDVIEWER_NAVIGATION_HPP_

#include <geometry/frame.hpp>

class Navigation final
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

  mode_t mode = IDLE;
  glm::vec3 turntable_origin = glm::vec3(0); // TODO: use

  frame_t navigate(frame_t view, glm::vec2 mouse_force, glm::vec3 key_force);

  void enableMode(mode_t mode);
  void disableMode(mode_t mode);
};

#endif // POINTCLOUDVIEWER_NAVIGATION_HPP_
