#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

#include <renderer/gl450/uniforms.hpp>

#include <QMouseEvent>
#include <QKeyEvent>

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

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
  const glm::ivec2 current_mouse_pos(event->x(), event->y());
  glm::vec2 mouse_force = glm::vec2(current_mouse_pos - last_mouse_pos) * 1.f;

  mouse_force = glm::clamp(glm::vec2(-20), glm::vec2(20), mouse_force);

  camera.frame = navigation.navigate(camera.frame, mouse_force, key_force);

  last_mouse_pos = current_mouse_pos;
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
  if(event->button() == Qt::MiddleButton)
  {
    last_mouse_pos = glm::ivec2(event->x(), event->y());

    if(event->modifiers() == Qt::NoModifier)
      navigation.enableMode(Navigation::TURNTABLE_ROTATE);
    else if(event->modifiers() == Qt::ShiftModifier)
      navigation.enableMode(Navigation::TURNTABLE_SHIFT);
    else if(event->modifiers() == Qt::ControlModifier)
      navigation.enableMode(Navigation::TURNTABLE_ZOOM);
  }
}

void Viewport::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() == Qt::MiddleButton)
  {
    navigation.disableMode(Navigation::TURNTABLE_ROTATE);
    navigation.disableMode(Navigation::TURNTABLE_SHIFT);
    navigation.disableMode(Navigation::TURNTABLE_ZOOM);
  }
}

void Viewport::keyPressEvent(QKeyEvent* event)
{
  // TODO update key_force
}

void Viewport::keyReleaseEvent(QKeyEvent* event)
{
  // TODO update key_force
}
