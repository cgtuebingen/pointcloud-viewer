#include <pointcloud_viewer/navigation.hpp>

frame_t Navigation::navigate(frame_t view, glm::vec2 mouse_force, glm::vec3 key_force) const
{
  if(mode == mode_t::IDLE)
    return view;

  const glm::vec3 forward = view.orientation * glm::vec3(0, 0, -1);
  const glm::vec3 up = view.orientation * glm::vec3(0, 1, 0);
  const glm::vec3 right = view.orientation * glm::vec3(1, 1, 0);

  switch(mode)
  {
  case FPS:
    view.position += forward * key_force.y + right * key_force.x;
    // TODO
    break;
  case TURNTABLE_ROTATE:
    // TODO
    break;
  case TURNTABLE_SHIFT:
    view.position += up * mouse_force.y + right * mouse_force.x;
    break;
  case TURNTABLE_ZOOM:
    // TODO
    break;
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
