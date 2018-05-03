#include <core_library/exception.hpp>
#include <pointcloud_viewer/mainwindow.hpp>

#include <iostream>

using namespace pointcloud_viewer;

int main(int argc, char** argv)
{
  try
  {
    mainwindow::Instance mainwindow_instance;

  }catch(exception_t exception)
  {
    std::cerr << exception.message;
    return -1;
  }
}
