#include <pointcloud_viewer/viewport.hpp>

#include <glhelper/gl.hpp>

Viewport::Viewport()
{
  QSurfaceFormat format;

  format.setVersion(4, 5);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setDepthBufferSize(24);

  setFormat(format);
  setMinimumSize(640, 480);
}

Viewport::~Viewport()
{
  delete point_renderer;
}

void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new render_system::PointRenderer();

  glm::vec4 bg_color(glm::vec3(0.25f), 1.f);

  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
}

void Viewport::resizeGL(int w, int h)
{
  Q_UNUSED(w);
  Q_UNUSED(h);
}

void Viewport::paintGL()
{
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  point_renderer->render_points();
}
