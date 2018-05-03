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

} // namespace mainwindow
} // namespace pointcloud_viewer

#endif // POINTCLOUDVIEWER_MAINWINDOW_HPP_
