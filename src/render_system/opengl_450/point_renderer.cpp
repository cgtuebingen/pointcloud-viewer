#include <glad/glad.h>
#include <glm/glm.hpp>

#include <render_system/point_renderer.hpp>

namespace render_system {

PointRenderer::PointRenderer()
{
}

PointRenderer::~PointRenderer()
{
}

void PointRenderer::render_points()
{
  glm::vec4 bg_color(glm::vec3(0.25f), 1.f);

  glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

} //namespace render_system
