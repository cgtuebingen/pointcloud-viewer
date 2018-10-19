#include <renderer/gl450/point_remapper.hpp>
#include <core_library/print.hpp>
#include <pointcloud/buffer.hpp>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

namespace renderer {
namespace gl450 {

void remap_points(const std::string& vertex_shader, const QVector<uint>& bindings, PointCloud* pointCloud)
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

  std::vector<gl::VertexArrayObject::Attribute> attributes;
  attributes.reserve(size_t(bindings.length()));
  size_t attribute_stride = pointCloud->user_data_stride;
  for(int i=0; i<bindings.length(); ++i)
  {
    QString property_name = pointCloud->user_data_names[i];
    data_type::base_type_t property_type = pointCloud->user_data_types[i];
    size_t property_offset = pointCloud->user_data_offset[i];

    auto attribute_binding = bindings[i];

    if(attribute_binding != std::numeric_limits<decltype(attribute_binding)>::max())
    {
      gl::VertexArrayObject::Attribute::Type attribute_type = gl::VertexArrayObject::Attribute::Type::INT8;

      switch(property_type)
      {
      case data_type::BASE_TYPE::FLOAT32:
        attribute_type = gl::VertexArrayObject::Attribute::Type::FLOAT;
        break;
      case data_type::BASE_TYPE::FLOAT64:
        attribute_type = gl::VertexArrayObject::Attribute::Type::DOUBLE;
        break;
      case data_type::BASE_TYPE::INT8:
        attribute_type = gl::VertexArrayObject::Attribute::Type::INT8;
        break;
      case data_type::BASE_TYPE::INT16:
        attribute_type = gl::VertexArrayObject::Attribute::Type::INT16;
        break;
      case data_type::BASE_TYPE::INT32:
        attribute_type = gl::VertexArrayObject::Attribute::Type::INT32;
        break;
      case data_type::BASE_TYPE::UINT8:
        attribute_type = gl::VertexArrayObject::Attribute::Type::UINT8;
        break;
      case data_type::BASE_TYPE::UINT16:
        attribute_type = gl::VertexArrayObject::Attribute::Type::UINT16;
        break;
      case data_type::BASE_TYPE::UINT32:
        attribute_type = gl::VertexArrayObject::Attribute::Type::UINT32;
        break;
      }

      attributes.push_back(gl::VertexArrayObject::Attribute(attribute_type, 1, attribute_binding));
    }
  }

  gl::VertexArrayObject vertex_array_object(std::move(attributes));


}

} //namespace gl450
} //namespace renderer
