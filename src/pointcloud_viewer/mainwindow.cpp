#include <pointcloud_viewer/mainwindow.hpp>
#include <core_library/exception.hpp>

#include <GLFW/glfw3.h>

namespace pointcloud_viewer {
namespace mainwindow {

GLFWwindow* window = nullptr;

Instance::Instance()
{
  window = glfwCreateWindow(640, 480, "Pointcloud Viewer", nullptr, nullptr);
  if(!window)
    throw exception_t{"Couldn't create glfw window"};
}

Instance::~Instance()
{
  glfwTerminate();

  window = nullptr;
}

} // namespace mainwindow
} // namespace pointcloud_viewer
