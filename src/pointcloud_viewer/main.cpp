#include <pointcloud_viewer/mainwindow.hpp>
#include <renderer/gl450/locate_shaders.hpp>

#include <fstream>

#include <QApplication>
#include <QSettings>

Q_DECLARE_METATYPE(glm::vec2)
Q_DECLARE_METATYPE(glm::vec3)
Q_DECLARE_METATYPE(glm::vec4)
Q_DECLARE_METATYPE(glm::ivec2)
Q_DECLARE_METATYPE(glm::ivec3)
Q_DECLARE_METATYPE(glm::ivec4)
Q_DECLARE_METATYPE(glm::uvec2)
Q_DECLARE_METATYPE(glm::uvec3)
Q_DECLARE_METATYPE(glm::uvec4)

int main(int argc, char** argv)
{
  qRegisterMetaType<glm::vec2>();
  qRegisterMetaType<glm::vec3>();
  qRegisterMetaType<glm::vec4>();
  qRegisterMetaType<glm::ivec2>();
  qRegisterMetaType<glm::ivec3>();
  qRegisterMetaType<glm::ivec4>();
  qRegisterMetaType<glm::uvec2>();
  qRegisterMetaType<glm::uvec3>();
  qRegisterMetaType<glm::uvec4>();

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
