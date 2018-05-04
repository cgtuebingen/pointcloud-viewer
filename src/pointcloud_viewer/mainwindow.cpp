#include <pointcloud_viewer/mainwindow.hpp>
#include <core_library/exception.hpp>

#include <GLFW/glfw3.h>

namespace pointcloud_viewer {
namespace mainwindow {

extern GLFWwindow* glfw_window;

void throw_glfw_error(int error, const char* description);

Instance::Instance()
{
  if(glfwInit() != GLFW_TRUE)
    throw exception_t{"Couldn't initialize glfw"};

  glfwSetErrorCallback(throw_glfw_error);

  glfw_window = glfwCreateWindow(640, 480, "Pointcloud Viewer", nullptr, nullptr);
  if(!glfw_window)
    throw exception_t{"Couldn't create glfw window"};
}

Instance::~Instance()
{
  glfwTerminate();

  glfw_window = nullptr;
}

GLFWwindow* glfw_window = nullptr;

void throw_glfw_error(int error, const char* description)
{
  print_error("glfw error(", error, "): ", description);
}

} // namespace mainwindow
} // namespace pointcloud_viewer
