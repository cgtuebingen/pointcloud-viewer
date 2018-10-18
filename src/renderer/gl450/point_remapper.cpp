#include <renderer/gl450/point_remapper.hpp>
#include <core_library/print.hpp>

namespace renderer {
namespace gl450 {

void remap_points(const std::string& vertex_shader, PointCloud* pointCloud)
{
  gl::ShaderObject shader_object("point_remapper");
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX, vertex_shader,"generated vertex shader");
  if(gl::Result::FAILURE == shader_object.CreateProgram())
  {
    println_error("==== Shader Code ================================");

    const bool with_linenumbers = true;
    if(with_linenumbers)
    {
      int line_number = 1;
      for(QString line : QString::fromStdString(vertex_shader).split('\n'))
      {
        println_error(line_number, ":\t", line.toStdString());
        line_number++;
      }
    }else
    {
      println_error(vertex_shader);
    }

    println_error("=================================================");
  }

}

} //namespace gl450
} //namespace renderer
