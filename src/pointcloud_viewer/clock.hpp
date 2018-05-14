#ifndef POINTCLOUDVIEWER_CLOCK_HPP_
#define POINTCLOUDVIEWER_CLOCK_HPP_

#include <core_library/singleton.hpp>

namespace pointcloud_viewer {
namespace clock {

typedef float time_t;

time_t total_time();
time_t delta_time();

void update_time();

class Instance final : public Singleton<Instance>
{
public:
  Instance();
  ~Instance();
};

} // namespace clock
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_CLOCK_HPP_
