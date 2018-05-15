#include <glad/glad.h>
#include <glhelper/gl.hpp>
#include <glm/glm.hpp>

#include <render_system/point_renderer.hpp>

namespace render_system {

PointRenderer::PointRenderer(PointRenderer&& point_renderer)
  : implementation(point_renderer.implementation)
{
  point_renderer.implementation = nullptr;
}

PointRenderer& PointRenderer::operator=(PointRenderer&& point_renderer)
{
  std::swap(point_renderer.implementation, point_renderer.implementation);
  return *this;
}

} //namespace render_system
