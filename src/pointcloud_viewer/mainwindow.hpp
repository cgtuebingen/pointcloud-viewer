#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <glm/glm.hpp>
#include <functional>

#include <core_library/singleton.hpp>

namespace pointcloud_viewer {

/*
Instead of singleton classes, a namespace with global functions is used here.
*/
namespace mainwindow {

/*
A class using raii to initialize/deinitialize the resources of a window.
*/
class Instance final : public Singleton<Instance>
{
public:
  Instance();
  ~Instance();
};

bool is_open();

void swap_buffers();
void process_events();

glm::ivec2 framebuffer_size();

extern std::function<void(glm::ivec2)> on_framebuffer_size_changed;

} // namespace mainwindow
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
