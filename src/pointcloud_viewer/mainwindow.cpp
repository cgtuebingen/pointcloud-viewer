#include <pointcloud_viewer/mainwindow.hpp>
#include <core_library/exception.hpp>
#include <render_system/initialization.hpp>

#include <GLFW/glfw3.h>

namespace pointcloud_viewer {
namespace mainwindow {

extern GLFWwindow* glfw_window;

void throw_glfw_error(int error, const char* description);

void glfw_set_framebuffer_size_callback(GLFWwindow*, int w, int h);

// Constructor initializes glfw and an opengl 4.5 window
Instance::Instance()
{
  // Initialize glfw
  if(glfwInit() != GLFW_TRUE)
    throw exception_t{"Couldn't initialize glfw"};
  glfwSetErrorCallback(throw_glfw_error);

  // create a window with an opengl 4.5 context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfw_window = glfwCreateWindow(640, 480, "Pointcloud Viewer", nullptr, nullptr);
  if(!glfw_window)
    throw exception_t{"Couldn't create glfw window"};

  glfwMakeContextCurrent(glfw_window);

  render_system::initialize_gl(render_system::gl_proc_loader_t(glfwGetProcAddress));

  glfwSwapInterval(1); // enabled v-sync

  glfwSetFramebufferSizeCallback(glfw_window, glfw_set_framebuffer_size_callback);
}

// Constructor deinitializes glfw and destroays all windows
Instance::~Instance()
{
  glfwDestroyWindow(glfw_window);
  glfw_window = nullptr;

  glfwTerminate();
}

GLFWwindow* glfw_window = nullptr;

void throw_glfw_error(int error, const char* description)
{
  print_error("glfw error(", error, "): ", description);
}

bool is_open()
{
  return !glfwWindowShouldClose(glfw_window);
}

void swap_buffers()
{
  glfwSwapBuffers(glfw_window);
}

void process_events()
{
  glfwPollEvents();
}

glm::ivec2 framebuffer_size()
{
  glm::ivec2 size;

  glfwGetFramebufferSize(glfw_window, &size.x, &size.y);

  return size;
}

std::function<void(glm::ivec2)> on_framebuffer_size_changed = [](glm::ivec2){};

void glfw_set_framebuffer_size_callback(GLFWwindow*, int w, int h)
{
  on_framebuffer_size_changed(glm::ivec2(w, h));
}

} // namespace mainwindow
} // namespace pointcloud_viewer
