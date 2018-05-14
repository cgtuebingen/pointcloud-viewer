#ifndef POINTCLOUDVIEWER_CLOCK_HPP_
#define POINTCLOUDVIEWER_CLOCK_HPP_

namespace pointcloud_viewer {
namespace clock {

typedef float time_t;

time_t total_time();
time_t delta_time();

void update_time();

} // namespace clock
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_CLOCK_HPP_
