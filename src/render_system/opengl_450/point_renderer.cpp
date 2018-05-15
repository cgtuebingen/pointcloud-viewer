#include <glad/glad.h>
#include <glhelper/gl.hpp>
#include <glm/glm.hpp>

#include <render_system/point_renderer.hpp>

namespace render_system {

void clear_color();

PointRenderer::PointRenderer()
{
}

PointRenderer::~PointRenderer()
{
}

void PointRenderer::render_points()
{
  clear_color();
}

void clear_color()
{
  glm::vec4 bg_color(glm::vec3(0.25f), 1.f);

  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT);
}

} //namespace render_system
