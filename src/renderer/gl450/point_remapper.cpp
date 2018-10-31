#include <renderer/gl450/point_remapper.hpp>
#include <core_library/print.hpp>
#include <pointcloud/buffer.hpp>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

#include <QSharedPointer>


extern QSet<QString> find_used_properties(const PointCloud* pointcloud);
extern PointCloud::Shader generate_code_from_shader(const PointCloud* pointcloud);

namespace renderer {
namespace gl450 {

enum class value_type_t;

constexpr const uint invalid_binding = std::numeric_limits<uint>::max();

std::tuple<QString, QVector<uint>> shader_code_glsl450(const PointCloud* pointcloud, QSet<QString> used_properties);
bool remap_points(const std::string& vertex_shader, const QVector<uint>& bindings, PointCloud* pointCloud);

bool remap_points(PointCloud* pointCloud)
{
  Q_ASSERT(pointCloud != nullptr);

  const QSet<QString> used_properties = find_used_properties(pointCloud);
  QString code;
  QVector<uint> property_vao_bindings;

  std::tie(code, property_vao_bindings) = shader_code_glsl450(pointCloud, used_properties);

  return remap_points(code.toStdString(), property_vao_bindings, pointCloud);
}

bool remap_points(const std::string& vertex_shader, const QVector<uint>& bindings, PointCloud* pointCloud)
{
  gl::ShaderObject shader_object("point_remapper");
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX, vertex_shader,"generated vertex shader");

  const bool failed = gl::Result::FAILURE == shader_object.CreateProgram();

  constexpr const bool always_print_shader = false;
  constexpr const bool with_linenumbers = true;

  if(failed || always_print_shader)
  {
    println_error("==== Shader Code ================================");

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

    if(failed)
      return false;
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

  const GLsizei points_per_block = glm::min(65536, num_points);

  gl::Buffer input_buffer(points_per_block * attribute_stride,
                          gl::Buffer::UsageFlag(gl::Buffer::MAP_WRITE | gl::Buffer::SUB_DATA_UPDATE));
  gl::Buffer output_buffer(points_per_block * vertex_stride,
                          gl::Buffer::UsageFlag(gl::Buffer::MAP_READ | gl::Buffer::SUB_DATA_UPDATE));

  vertex_array_object.Bind();
  for(int i=0, binding_index=0; i<bindings.length(); ++i)
  {
    if(bindings[i] != invalid_binding)
    {
      size_t property_offset = pointCloud->user_data_offset[i];
      input_buffer.BindVertexBuffer(uint(binding_index++), GLsizeiptr(property_offset), attribute_stride);
    }
  }

  auto remap_block = [&output_buffer, &shader_object, &input_buffer, &pointCloud, attribute_stride](GLintptr first_index, GLintptr num_vertices)
  {
    input_buffer.Set(pointCloud->user_data.data() + first_index * attribute_stride, 0, num_vertices * attribute_stride);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    output_buffer.BindShaderStorageBuffer(0, 0, num_vertices * vertex_stride);


    shader_object.Activate();

    GL_CALL(glDrawArrays, GL_POINTS, 0, num_vertices);

    shader_object.Deactivate();

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    output_buffer.Get(pointCloud->coordinate_color.data() + first_index * vertex_stride, 0, num_vertices * vertex_stride);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
  };

  for(GLintptr i=0; i<num_points; i+=points_per_block)
  {
    remap_block(i, glm::min<GLintptr>(i+points_per_block, num_points) - i);
  }


  for(int i=0; i<bindings.length(); ++i)
  {
    if(bindings[i] != invalid_binding)
    {
      glBindVertexBuffer(bindings[i], 0, 0, 0);
    }
  }
  vertex_array_object.ResetBinding();

  return true;
}

std::tuple<QString, QVector<uint>> shader_code_glsl450(const PointCloud* pointcloud, QSet<QString> used_properties)
{
  QString code;
  code += "#version 450 core\n";
  code += "\n";

  if(pointcloud == nullptr)
  {
    code += "void main()\n{\n}\n";
    return std::make_tuple(code, QVector<uint>());
  }

  // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
  // https://www.khronos.org/files/opengl45-quick-reference-card.pdf

  PointCloud::Shader shader = generate_code_from_shader(pointcloud);

  if(shader.coordinate_expression.isEmpty())
    shader.coordinate_expression = "vec3(0) /* not set */";
  if(shader.color_expression.isEmpty())
    shader.color_expression = "uvec3(255) /* not set */";

  code += "\n";
  code += "// ==== Input Buffer ====\n";
  uint binding_index = 0;
  QVector<decltype(binding_index)> property_bindings;
  for(int i=0; i<pointcloud->user_data_names.length(); ++i)
  {
    QString type = property_to_glsl_type(pointcloud->user_data_types[i]);
    QString name = pointcloud->user_data_names[i];

    if(used_properties.contains(name) == false)
    {
      property_bindings << invalid_binding;
      continue;
    }

    property_bindings << binding_index;

    code += "layout(location = " + QString::number(binding_index)+ ")\n";
    code += "in " + type + " " + name + ";\n";

    binding_index++;
  }
  code += "\n";

  code += "// ==== Output Buffer ====\n";
  code += "struct vertex_t\n";
  code += "{\n";
  code += "  vec3 coordinate;\n";
  code += "  uint color;\n";
  code += "};\n";
  code += "\n";
  code += "layout(std430, binding=0)\n";
  code += "buffer impl_output_buffer\n";
  code += "{\n";
  code += "  vertex_t impl_output_vertex[];\n";
  code += "};\n";
  code += "\n";

  code += "// ==== Helper Functions ====\n";
  code += "int    to_scalar(in ivec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "uint   to_scalar(in uvec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "float  to_scalar(in  vec3 v){return (v.x + v.y + v.z) / 3;}\n";
  code += "double to_scalar(in dvec3 v){return (v.x + v.y + v.z) / 3;}\n";
  for(const char* t : {"int", "uint", "float", "double", "ivec3", "uvec3", "vec3", "dvec3"})
  {
    QString expression;
    QString fn = "${type} handle_switch(int condition";
    for(int i=0; i<10; ++i)
    {
      fn += QString(", int case%0, ${type} value%0").arg(i);

      expression += QString("condition == case%0 ? value%0 : ").arg(i);
    }
    expression += "default_value";
    fn += ", ${type} default_value){return " + expression + ";}\n";
    code += fn.replace("${type}", t);
  }
  code += "\n";
  code += "// ==== Actual execution ====\n";
  code += "void main()\n";
  code += "{\n";
  code += "  impl_output_vertex[gl_VertexID].coordinate = \n";
  code += "\n"
          "      // ==== COORDINATE ====\n"
          "      " + shader.coordinate_expression + ";\n"
          "      // ====================\n";
  code += "\n";
  code += "  impl_output_vertex[gl_VertexID].color = packUnorm4x8(vec4(vec3(\n";
  code += "\n";
  code += "      // ==== COLOR =========\n";
  code += "      " + shader.color_expression + "\n";
  code += "      // ====================\n";
  code += "\n  ), 0) / 255.);\n";
  code += "}\n";

  return std::make_tuple(code, property_bindings);
}

} //namespace gl450
} //namespace renderer
