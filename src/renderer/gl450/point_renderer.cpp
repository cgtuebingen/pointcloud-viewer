#include <renderer/gl450/point_renderer.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/io.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>
#include <core_library/padding.hpp>

namespace renderer {
namespace gl450 {

struct vertex_t
{
  glm::vec3 coordinate;
  glm::u8vec3 color;
  padding<uint8_t> _padding;
};

const int COLOR_OFFSET = 3*4;
const GLsizeiptr STRIDE = 4*4;

const int POSITION_BINDING_INDEX = 0;
const int COLOR_BINDING_INDEX = 1;


PointRenderer::PointRenderer()
  : shader_object("point_renderer"),
    vertex_array_object({gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::FLOAT, 3, POSITION_BINDING_INDEX),
                        gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::UINT8, 3, COLOR_BINDING_INDEX, gl::VertexArrayObject::Attribute::IntegerHandling::NORMALIZED),
})
{
  shader_object.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX,
                                  "point_cloud.vs.glsl",
                                  format("#define POSITION_BINDING_INDEX ", POSITION_BINDING_INDEX, "\n",
                                         "#define COLOR_BINDING_INDEX ", COLOR_BINDING_INDEX, "\n"));
  shader_object.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT,
                                  "point_cloud.fs.glsl");
  shader_object.CreateProgram();

}

PointRenderer::~PointRenderer()
{
}

PointRenderer::PointRenderer(PointRenderer&& point_renderer)
  : shader_object(std::move(point_renderer.shader_object)),
    vertex_position_buffer(std::move(point_renderer.vertex_position_buffer)),
    vertex_array_object(std::move(point_renderer.vertex_array_object))
{
}

PointRenderer& PointRenderer::operator=(PointRenderer&& point_renderer)
{
  shader_object = std::move(point_renderer.shader_object);
  vertex_position_buffer = std::move(point_renderer.vertex_position_buffer);
  vertex_array_object = std::move(point_renderer.vertex_array_object);
  return *this;
}

void PointRenderer::load_points(const uint8_t* point_data, GLsizei num_points)
{
  gl::Buffer buffer(GLsizeiptr(num_points) * STRIDE, gl::Buffer::UsageFlag::IMMUTABLE, point_data);

  this->vertex_position_buffer = std::move(buffer);
  this->num_vertices = num_points;

  const vertex_t* vertices = reinterpret_cast<const vertex_t*>(point_data);
  for(GLsizei i=0; i<num_points; ++i)
    print(vertices[i].coordinate, " ", vertices[i].color);
}

void PointRenderer::load_test(GLsizei num_vertices)
{
  gl::Buffer buffer(GLsizeiptr(num_vertices) * STRIDE, gl::Buffer::UsageFlag::MAP_WRITE, nullptr);

  vertex_t* vertices = reinterpret_cast<vertex_t*>(buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  for(GLsizei i=0; i<num_vertices; ++i)
  {
    float angle = glm::two_pi<float>() * i / float(num_vertices);

    vertices[i].coordinate = glm::vec3(glm::cos(angle), glm::sin(angle), 0.f);
    vertices[i].color = glm::u8vec3(255, 128, 0);
  }
  vertices = nullptr;
  buffer.Unmap();

  this->vertex_position_buffer = std::move(buffer);
  this->num_vertices = num_vertices;
}

void PointRenderer::render_points()
{
  if(Q_UNLIKELY(num_vertices == 0))
    return;

  vertex_array_object.Bind();
  vertex_position_buffer.BindVertexBuffer(POSITION_BINDING_INDEX, 0, STRIDE);
  vertex_position_buffer.BindVertexBuffer(COLOR_BINDING_INDEX, COLOR_OFFSET, STRIDE);

  shader_object.Activate();
  GL_CALL(glDrawArrays, GL_POINTS, 0, num_vertices);
  shader_object.Deactivate();
}

} //namespace gl450
} //namespace renderer
