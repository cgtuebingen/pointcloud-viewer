#include <core_library/exception.hpp>
#include <pointcloud_viewer/mainwindow.hpp>
#include <pointcloud_viewer/clock.hpp>
#include <render_system/point_renderer.hpp>

#include <iostream>

using namespace pointcloud_viewer;

int main(int argc, char** argv)
{
  try
  {
    mainwindow::Instance mainwindow_instance;
    clock::Instance clock_instance;

    render_system::PointRenderer point_renderer;

    while(mainwindow::is_open())
    {
      point_renderer.render_points();

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
