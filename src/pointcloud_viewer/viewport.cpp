#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

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
  delete visualization;
}

// Called by Qt right after the OpenGL context was created
void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new PointRenderer();
  visualization = new Visualization();

  glm::vec4 bg_color = color_palette::grey[0];
  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
}

// Called by Qt everytime the opengl window was resized
void Viewport::resizeGL(int w, int h)
{
  camera.aspect = float(w) / float(h);
}

// Called by Qt everytime the opengl window needs to be repainted
void Viewport::paintGL()
{
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  const glm::mat4 camera_matrix = camera.view_perspective_matrix();

  // TODO add the camera matrix to a global uniform

  visualization->render();
  point_renderer->render_points();
}
