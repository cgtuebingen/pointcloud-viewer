#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

#include <renderer/gl450/uniforms.hpp>
#include <renderer/gl450/point_remapper.hpp>

#include <QElapsedTimer>
#include <QSettings>
#include <QPainter>
#include <QMessageBox>

Viewport::Viewport()
  : navigation(this)
{
  QSurfaceFormat format;

  format.setVersion(4, 5);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setDepthBufferSize(24);

  m_backgroundColor = glm::clamp(int(glm::round(glm::vec3(color_palette::grey[0]).g * 255.f)), 0, 255);

  setFormat(format);
  setMinimumSize(640, 480);

  QSettings settings;
  m_pointSize = settings.value("Rendering/pointSize", 1.f).value<int>();
  m_backgroundColor = settings.value("Rendering/backgroundColor", m_backgroundColor).value<int>();
}

Viewport::~Viewport()
{
  delete global_uniform;
  delete point_renderer;
  delete _visualization;

  QSettings settings;
  settings.setValue("Rendering/pointSize", int(m_pointSize));
  settings.setValue("Rendering/backgroundColor", m_backgroundColor);
}

aabb_t Viewport::aabb() const
{
  return _aabb;
}

void Viewport::set_camera_frame(const frame_t& frame)
{
  navigation.camera.frame = frame;
  update();
}

void Viewport::unload_all_point_clouds()
{
  point_renderer->clear_buffer();
  _aabb = aabb_t::invalid();
  this->point_cloud.clear();

  this->update();
}

void Viewport::load_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;

  _aabb = point_cloud->aabb;

  this->makeCurrent();
  point_renderer->load_points(point_cloud->coordinate_color.data(), GLsizei(point_cloud->num_points));
  this->doneCurrent();

  this->update();
}

bool Viewport::reapply_point_shader()
{
  this->makeCurrent();

  if(!renderer::gl450::remap_points(point_cloud))
  {
    this->doneCurrent();
    QMessageBox::warning(this, "Shader error", "Could not apply the point shader.\nPlease take a look at the Standard Output");
    return false;
  }

  point_renderer->load_points(point_cloud->coordinate_color.data(), GLsizei(point_cloud->num_points));

  this->doneCurrent();

  return true;
}

void Viewport::render_points(frame_t camera_frame, float aspect, std::function<void ()> additional_rendering) const
{
  GL_CALL(glClearColor, m_backgroundColor/255.f, m_backgroundColor/255.f, m_backgroundColor/255.f, 1.f);
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  GL_CALL(glDepthFunc, GL_LEQUAL);
  GL_CALL(glEnable, GL_DEPTH_TEST);
  GL_CALL(glPointSize, m_pointSize);

  Camera camera = navigation.camera;
  camera.aspect = aspect;
  camera.frame = camera_frame;

  // Update the global uniforms
  GlobalUniform::vertex_data_t global_vertex_data;
  global_vertex_data.camera_matrix = camera.view_perspective_matrix();
  global_uniform->write(global_vertex_data);
  global_uniform->bind();

  point_renderer->render_points();
  additional_rendering();

  global_uniform->unbind();
}

int Viewport::backgroundColor() const
{
  return m_backgroundColor;
}

int Viewport::pointSize() const
{
  return m_pointSize;
}

void Viewport::setBackgroundColor(int backgroundColor)
{
  if(m_backgroundColor == backgroundColor)
    return;

  m_backgroundColor = backgroundColor;
  emit backgroundColorChanged(m_backgroundColor);

  update();
}

void Viewport::setPointSize(int pointSize)
{
  if(m_pointSize == pointSize)
    return;

  pointSize = glm::clamp<int>(pointSize, 1, 16);

  m_pointSize = pointSize;
  emit pointSizeChanged(m_pointSize);
  update();
}

// Called by Qt right after the OpenGL context was created
void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new PointRenderer();
  global_uniform = new GlobalUniform();
  _visualization = new Visualization();

  //  point_renderer->load_test();

  makeCurrent();
  openGlContextCreated();
  doneCurrent();
}

// Called by Qt everytime the opengl window was resized
void Viewport::resizeGL(int w, int h)
{
  navigation.camera.aspect = float(w) / float(h);
}

// Called by Qt everytime the opengl window needs to be repainted
void Viewport::paintGL()
{
  QElapsedTimer timer;
  timer.start();

  if(enable_preview)
  {
    render_points(navigation.camera.frame, navigation.camera.aspect, [this](){
      visualization().render();
    });
  }

  frame_rendered(timer.nsecsElapsed() * 1.e-9);
}

void Viewport::paintEvent(QPaintEvent* event)
{
  QOpenGLWidget::paintEvent(event);

  {
    QPainter painter(this);
    visualization().draw_overlay(painter, navigation.camera, pointSize(), glm::ivec2(this->width(), this->height()));
  }
}

void Viewport::wheelEvent(QWheelEvent* event)
{
  navigation.wheelEvent(event);
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
  navigation.mouseMoveEvent(event);
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
  navigation.mousePressEvent(event);
}

void Viewport::mouseReleaseEvent(QMouseEvent* event)
{
  navigation.mouseReleaseEvent(event);
}

void Viewport::mouseDoubleClickEvent(QMouseEvent* event)
{
  navigation.mouseDoubleClickEvent(event);
}

void Viewport::keyPressEvent(QKeyEvent* event)
{
  navigation.keyPressEvent(event);
}

void Viewport::keyReleaseEvent(QKeyEvent* event)
{
  navigation.keyReleaseEvent(event);
}
