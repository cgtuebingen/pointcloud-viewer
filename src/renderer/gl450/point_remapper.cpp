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
  const uint invalid_binding = std::numeric_limits<uint>::max();

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

  GLsizei num_points = GLsizei(pointCloud->num_points);

  std::vector<gl::VertexArrayObject::Attribute> attributes;
  attributes.reserve(size_t(bindings.length()));
  const GLsizei attribute_stride = GLsizei(pointCloud->user_data_stride);
  for(int i=0; i<bindings.length(); ++i)
  {
    QString property_name = pointCloud->user_data_names[i];
    data_type::base_type_t property_type = pointCloud->user_data_types[i];

    uint attribute_binding = bindings[i];

    if(attribute_binding != invalid_binding)
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

  constexpr const GLsizeiptr vertex_stride = sizeof(glm::vec3) + sizeof(glm::u8vec4);

  gl::VertexArrayObject vertex_array_object(std::move(attributes));

  // TODO do this blockwise for huge pointclouds?

  gl::Buffer input_buffer(num_points * attribute_stride,
                          gl::Buffer::UsageFlag(gl::Buffer::MAP_WRITE | gl::Buffer::MAP_PERSISTENT | gl::Buffer::MAP_COHERENT),
                          pointCloud->user_data.data());
  gl::Buffer output_buffer(num_points * vertex_stride,
                          gl::Buffer::UsageFlag(gl::Buffer::MAP_READ | gl::Buffer::SUB_DATA_UPDATE));

  vertex_array_object.Bind();
  for(int i=0; i<bindings.length(); ++i)
  {
    if(bindings[i] != invalid_binding)
    {
      size_t property_offset = pointCloud->user_data_offset[i];
      input_buffer.BindVertexBuffer(uint(i), GLsizeiptr(property_offset), attribute_stride);
    }
  }

  output_buffer.BindShaderStorageBuffer(0);

  shader_object.Activate();

  GL_CALL(glDrawArrays, GL_POINTS, 0, num_points);
  shader_object.Deactivate();

  output_buffer.Get(pointCloud->coordinate_color.data(), 0, output_buffer.GetSize());

  for(int i=0; i<bindings.length(); ++i)
  {
    if(bindings[i] != invalid_binding)
    {
      glBindVertexBuffer(bindings[i], 0, 0, 0);
    }
  }
  vertex_array_object.ResetBinding();
}

} //namespace gl450
} //namespace renderer
