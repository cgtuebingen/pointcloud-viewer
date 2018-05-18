#include <pointcloud_viewer/viewport.hpp>

#include <renderer/gl450/point_renderer.hpp>
#include <renderer/gl450/debug/debug_mesh.hpp>

class Viewport::Visualization
{
public:
  Visualization();

  void render();

private:
  typedef render_system::gl450::DebugMeshRenderer DebugMeshRenderer;
  typedef render_system::gl450::DebugMesh DebugMesh;

  DebugMeshRenderer debug_mesh_renderer;

  DebugMesh world_axis;
};

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

void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new PointRenderer();
  visualization = new Visualization();

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
  visualization->render();
  point_renderer->render_points();
}

Viewport::Visualization::Visualization()
  : world_axis(DebugMesh::axis(glm::bvec3(true), 0.7f))
{
}

void Viewport::Visualization::render()
{
  debug_mesh_renderer.begin();
  debug_mesh_renderer.render(world_axis);
  debug_mesh_renderer.end();
}
