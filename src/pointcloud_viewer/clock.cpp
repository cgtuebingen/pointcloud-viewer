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
  return _total_time;
}

time_t delta_time()
{
  return _total_time;
}

void reset_time()
{
  glfwSetTime(0.);

  _total_time = 0.;
}

void update_time()
{
  time_t last_time = _total_time;
  time_t new_time = time_t(glfwGetTime());

  _total_time = last_time;
  _delta_time = new_time-last_time;

  if(UNLIKELY(_delta_time < 2.e-3f))
    _delta_time = 2.e-3f;
}

} // namespace clock
} // namespace pointcloud_viewer
