#include <core_library/exception.hpp>
#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/clock.hpp>

#include <iostream>

using namespace pointcloud_viewer;

int main(int argc, char** argv)
{
  try
  {
    mainwindow::Instance mainwindow_instance;
    clock::Instance clock_instance;

    while(mainwindow::is_open())
    {
      //glClear(GL_COLOR_BUFFER_BIT); TODO call this using the render_system

      mainwindow::swap_buffers();

      mainwindow::process_events();
      clock::update_time();
    }

  }catch(exception_t exception)
  {
    print_error(exception.message);
    return -1;
  }
}
