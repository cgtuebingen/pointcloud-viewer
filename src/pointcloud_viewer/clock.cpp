#include <GLFW/glfw3.h>
#include <pointcloud_viewer/clock.hpp>
#include <core_library/macros.hpp>

namespace pointcloud_viewer {
namespace clock {

extern time_t _total_time;
extern time_t _delta_time;

time_t _total_time = 0.;
time_t _delta_time = 1.f/60.f;

time_t total_time()
{
  assert(Instance::is_initalized());

  return _total_time;
}

time_t delta_time()
{
  assert(Instance::is_initalized());

  return _delta_time;
}

void update_time()
{
  assert(Instance::is_initalized());

  time_t last_time = _total_time;
  time_t new_time = time_t(glfwGetTime());

  _total_time = last_time;
  _delta_time = new_time-last_time;

  // prevent too huge jumps, if the framerate gets too small
  const time_t delta_time_limit = 0.2f;
  if(UNLIKELY(_delta_time > delta_time_limit))
    _delta_time = delta_time_limit;
}

Instance::Instance()
{
  glfwSetTime(0.);

  _total_time = 0.;
}

Instance::~Instance()
{
}

} // namespace clock
} // namespace pointcloud_viewer
