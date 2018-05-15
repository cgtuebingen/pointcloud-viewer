#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

#include <core_library/string.hpp>
#include <render_system/point_renderer.hpp>

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

  void clear_color();
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

PointRenderer::Implementation::Implementation()
  : shader_object("point_renderer")
{
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                    format("#version 450 core\n"
                                           "\n"
                                           "#define NUM_VERTICES ", NUM_VERTICES, "\n",
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  float angle = gl_VertexID / float(NUM_VERTICES);\n"
                                           "  gl_Position = vec4(cos(angle), sin(angle), 0, 1);\n"
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
}

PointRenderer::Implementation::~Implementation()
{
}

void PointRenderer::Implementation::render_points()
{
  clear_color();
}

void PointRenderer::Implementation::clear_color()
{
  glm::vec4 bg_color(glm::vec3(0.25f), 1.f);

  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT);
}

} //namespace render_system
