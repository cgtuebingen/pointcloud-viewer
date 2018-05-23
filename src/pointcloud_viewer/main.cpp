#include <pointcloud_viewer/mainwindow.hpp>
#include <renderer/gl450/locate_shaders.hpp>

#include <QApplication>

int main(int argc, char** argv)
{
  QApplication application(argc, argv);

  renderer::gl450::locate_shaders();

  MainWindow mainwindow;

  mainwindow.show();

  application.exec();
}
