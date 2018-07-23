#include <pointcloud_viewer/mainwindow.hpp>
#include <renderer/gl450/locate_shaders.hpp>

#include <fstream>

#include <QApplication>
#include <QSettings>

int main(int argc, char** argv)
{
  QApplication application(argc, argv);

  QCoreApplication::setOrganizationName("cgi-uni-tuebingen");
  QCoreApplication::setApplicationName("pointcloud-viewer");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  renderer::gl450::locate_shaders();

  MainWindow mainwindow;

  mainwindow.show();

  if(mainwindow.noninteractive == false)
    application.exec();
}
