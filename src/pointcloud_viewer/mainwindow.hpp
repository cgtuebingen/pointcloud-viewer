#include <GLFW/glfw3.h>

#include <core_library/singleton.hpp>

namespace pointcloud_viewer {
namespace mainwindow {

class Instance : public Singleton<Instance>
{
public:
  Instance();
  ~Instance();
};

} // namespace mainwindow
} // namespace pointcloud_viewer
