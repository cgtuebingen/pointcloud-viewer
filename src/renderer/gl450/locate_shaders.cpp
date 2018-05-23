#include <core_library/print.hpp>
#include <renderer/gl450/locate_shaders.hpp>
#include <glhelper/gl.hpp>

#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

namespace renderer {
namespace gl450 {

void locate_shaders()
{
  QVector<QDir> candidate_dirs;

  QDir binaryDirectory = QCoreApplication::applicationDirPath();

  if(QFileInfo(__FILE__).exists())
    candidate_dirs << QFileInfo(__FILE__).dir().absoluteFilePath("shader");

  for(QDir candidate : candidate_dirs)
  {
    if(candidate.exists() && candidate.exists("uniforms/global_vertex.glsl"))
    {
      gl::Details::ShaderIncludeDirManager::addIncludeDirs(candidate);
      return;
    }
  }

  print_error("Couldn't locate the shader files."
              "Please copy the shaders directory from ",
              QFileInfo(__FILE__).dir().absolutePath().toStdString(),
              " to ./shaders");
  std::abort();
}

} //namespace gl450
} //namespace renderer
