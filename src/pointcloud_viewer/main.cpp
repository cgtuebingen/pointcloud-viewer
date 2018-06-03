#include <pointcloud_viewer/mainwindow.hpp>
#include <renderer/gl450/locate_shaders.hpp>

#include <tinyply.h>
#include <fstream>

#include <QApplication>

int main(int argc, char** argv)
{
  /*
  tinyply::PlyFile file;

  std::ifstream filestream("/home/hildebrr/Desktop/area1_rgb.ply", std::ios::in);
  file.parse_header(filestream);
  file.request_properties_from_element("vertex", {"x", "y", "z"});
  // TODO I have the full contoll over the stream! I can copy blocks of the ply file to the memory and read them with file.read? Can I? THis way, I can have a progress bar!! :D
  // file.read(filestream);
  */

  QApplication application(argc, argv);

  renderer::gl450::locate_shaders();

  MainWindow mainwindow;

  mainwindow.show();

  application.exec();
}
