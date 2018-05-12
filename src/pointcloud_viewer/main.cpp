#include <core_library/exception.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/io.hpp>

#include <iostream>

using namespace pointcloud_viewer;

int main(int argc, char** argv)
{
  try
  {
    mainwindow::on_framebuffer_size_changed = [](glm::ivec2 size){print("framebuffer size: ", size);};

    mainwindow::Instance mainwindow_instance;

    while(mainwindow::is_open())
    {
      glClear(GL_COLOR_BUFFER_BIT);

      mainwindow::swap_buffers();

      mainwindow::process_events();
    }

  }catch(exception_t exception)
  {
    print_error(exception.message);
    return -1;
  }
}
