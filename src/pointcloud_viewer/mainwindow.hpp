#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <glm/glm.hpp>
#include <functional>

#include <core_library/singleton.hpp>

namespace pointcloud_viewer {

/*
Instead of singletons, a namespace with global functions is used here.
*/
namespace mainwindow {

/*
A class using raii to initialize/deinitialize the resources of a window.
*/
class Instance : public Singleton<Instance>
{
public:
  Instance();
  ~Instance();
};

/*
Returns true, if the window is still open.

Use to determine, whether to close the window.
*/
bool is_open();

/**
Swap the back and front buffer
*/
void swap_buffers();

/**
Poll and process window events
*/
void process_events();

/**
The size of the framebuffer of the window.
*/
glm::ivec2 framebuffer_size();

extern std::function<void(glm::ivec2)> on_framebuffer_size_changed;

} // namespace mainwindow
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
