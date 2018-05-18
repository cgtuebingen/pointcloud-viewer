#include <pointcloud_viewer/viewport.hpp>

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
}

void Viewport::resizeGL(int w, int h)
{
  Q_UNUSED(w);
  Q_UNUSED(h);
}

void Viewport::paintGL()
{
  point_renderer->render_points();
}
