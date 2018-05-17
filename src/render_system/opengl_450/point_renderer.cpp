#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

#include <core_library/string.hpp>
#include <render_system/point_renderer.hpp>
#include <glm/gtc/constants.hpp>

namespace render_system {


class PointRenderer::Implementation final
{
public:
  nocopy(Implementation)

  Implementation();
  ~Implementation();

  void render_points();

private:
  gl::ShaderObject shader_object;
  gl::Buffer vertex_position_buffer;
  gl::VertexArrayObject vertex_array_object;

  void clear_color();
  void render();
};

PointRenderer::PointRenderer()
{
  implementation = new Implementation;
}

PointRenderer::~PointRenderer()
{
  delete implementation;
}

void PointRenderer::render_points()
{
  implementation->render_points();
}

// ======== PointRenderer::Implementation ======================================

const int NUM_VERTICES = 1024;
const int POSITION_BINDING_INDEX = 0;

PointRenderer::Implementation::Implementation()
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
                                           "out vec4 color;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  color = vec4(1, 0.5, 0, 1);\n"
                                           "}\n"),
                                    "PointRenderer::Implementation::Implementation() // fragment");

  glm::vec4* vertex = reinterpret_cast<glm::vec4*>(vertex_position_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  for(int i=0; i<NUM_VERTICES; ++i)
  {
    float angle = glm::two_pi<float>() * i / float(NUM_VERTICES);
    vertex[i] = glm::vec4(glm::cos(angle), glm::sin(angle), 0.f, 1.f);
  }
  vertex_position_buffer.Unmap();
  vertex = nullptr;
}

PointRenderer::Implementation::~Implementation()
{
}

void PointRenderer::Implementation::render_points()
{
  clear_color();
  render();
}

void PointRenderer::Implementation::clear_color()
{
  glm::vec4 bg_color(glm::vec3(0.25f), 1.f);

  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void PointRenderer::Implementation::render()
{
  vertex_array_object.Bind();
  vertex_position_buffer.BindVertexBuffer(POSITION_BINDING_INDEX, 0, vertex_array_object.GetVertexStride());

  shader_object.Activate();
  GL_CALL(glDrawArrays, GL_LINE_STRIP, 0, NUM_VERTICES);
}

} //namespace render_system
