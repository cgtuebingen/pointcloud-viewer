#include <pointcloud_viewer/navigation.hpp>
#include <core_library/print.hpp>

#include <glm/gtx/io.hpp>

frame_t Navigation::navigate(frame_t view, glm::vec2 mouse_force, glm::vec3 key_force) const
{
  if(mode == mode_t::IDLE)
    return view;

  print("mouse_force ", mouse_force, "   key_force ", key_force);

  const glm::vec3 forward = view.orientation * glm::vec3(0, 0, -1);
  const glm::vec3 up = view.orientation * glm::vec3(0, 1, 0);
  const glm::vec3 right = view.orientation * glm::vec3(1, 0, 0);

  switch(mode)
  {
  case FPS:
    view.position += forward * key_force.y + right * key_force.x;
    // TODO
    break;
  case TURNTABLE_ROTATE:
    view.position -= turntable_origin;
    view = frame_t(turntable_origin, glm::angleAxis(-mouse_force.x, glm::vec3(0,0,1)) * glm::angleAxis(-mouse_force.y, right)) * view;
    // TODOturntable_origin
    break;
  case TURNTABLE_SHIFT:
    view.position += - up * mouse_force.y + right * mouse_force.x;
    break;
  case TURNTABLE_ZOOM:
  {
    glm::vec3 previous_zoom = view.position - turntable_origin;

    float zoom_factor = glm::clamp(0.5f, 1.5f, glm::exp2(mouse_force.y));

    view.position = turntable_origin + zoom_factor * previous_zoom;
    break;
  }
  case IDLE:
    break;
  }

  return view;
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
