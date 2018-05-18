#include <renderer/gl450/point_renderer.hpp>

#include <glm/gtc/constants.hpp>
#include <core_library/print.hpp>

namespace renderer {
namespace gl450 {

const int NUM_VERTICES = 512;
const int POSITION_BINDING_INDEX = 0;

PointRenderer::PointRenderer()
  : shader_object("point_renderer"),
    vertex_position_buffer(NUM_VERTICES * sizeof(glm::vec4), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    vertex_array_object({gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::FLOAT, 4, POSITION_BINDING_INDEX)})
{
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                    format("#version 450 core\n"
                                           "\n"
                                           "layout(location = ", POSITION_BINDING_INDEX, ")\n",
                                           "in vec4 point_coord;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  gl_Position = vec4(point_coord.xyz, 1);\n"
                                           "}\n"),
                                    "PointRenderer::Implementation::Implementation() // vertex");
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                                    format("#version 450 core\n"
                                           "\n"
                                           "layout(location=0)\n"
                                           "out vec4 color;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  color = vec4(1, 0.5, 0, 1);\n"
                                           "}\n"),
                                    "PointRenderer::Implementation::Implementation() // fragment");
  shader_object.CreateProgram();

  glm::vec4* vertex = reinterpret_cast<glm::vec4*>(vertex_position_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  for(int i=0; i<NUM_VERTICES; ++i)
  {
    float angle = glm::two_pi<float>() * i / float(NUM_VERTICES);
    vertex[i] = glm::vec4(glm::cos(angle), glm::sin(angle), 0.f, 1.f);
  }
  vertex_position_buffer.Unmap();
  vertex = nullptr;
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

void PointRenderer::render_points()
{
  vertex_array_object.Bind();
  vertex_position_buffer.BindVertexBuffer(POSITION_BINDING_INDEX, 0, static_cast<GLsizei>(vertex_array_object.GetVertexStride()));

  shader_object.Activate();
  GL_CALL(glDrawArrays, GL_POINTS, 0, NUM_VERTICES);
  shader_object.Deactivate();
}

} //namespace gl450
} //namespace renderer
