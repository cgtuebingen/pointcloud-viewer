#ifndef POINTCLOUDVIEWER_MAINWINDOW_HPP_
#define POINTCLOUDVIEWER_MAINWINDOW_HPP_

#include <GLFW/glfw3.h>

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

} // namespace mainwindow
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
