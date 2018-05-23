#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

#include <renderer/gl450/uniforms.hpp>

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
  delete global_uniform;
  delete point_renderer;
  delete visualization;
}

// Called by Qt right after the OpenGL context was created
void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new PointRenderer();
  global_uniform = new GlobalUniform();
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

  // Update the global uniforms
  GlobalUniform::vertex_data_t global_vertex_data;
  global_vertex_data.camera_matrix = camera.view_perspective_matrix();
  global_uniform->write(global_vertex_data);
  global_uniform->bind();


  visualization->render();
  point_renderer->render_points();

  global_uniform->unbind();
}
