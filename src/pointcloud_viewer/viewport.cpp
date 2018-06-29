#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

#include <renderer/gl450/uniforms.hpp>

Viewport::Viewport()
  : navigation(this)
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

void Viewport::set_camera_frame(const frame_t& frame)
{
  navigation.camera.frame = frame;
  update();
}

void Viewport::unload_all_point_clouds()
{
  point_renderer->clear_buffer();
}

point_cloud_handle_t Viewport::load_point_cloud(PointCloud&& point_cloud)
{
  point_cloud_handle_t handle = point_cloud_handle_t(next_handle++);
  PointCloud& p = point_clouds[size_t(handle)] = std::move(point_cloud);

  if(Q_UNLIKELY(p.num_points>std::numeric_limits<GLsizei>::max()))
    return point_cloud_handle_t::INVALID;

  point_renderer->load_points(p.coordinate_color.data(), GLsizei(p.num_points));

  this->update();

  return point_cloud_handle_t(handle);
}

// Called by Qt right after the OpenGL context was created
void Viewport::initializeGL()
{
  gladLoadGL();

  point_renderer = new PointRenderer();
  global_uniform = new GlobalUniform();
  visualization = new Visualization();

//  point_renderer->load_test();

  glm::vec4 bg_color = color_palette::grey[0];
  GL_CALL(glClearColor, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
}

// Called by Qt everytime the opengl window was resized
void Viewport::resizeGL(int w, int h)
{
  navigation.camera.aspect = float(w) / float(h);
}

// Called by Qt everytime the opengl window needs to be repainted
void Viewport::paintGL()
{
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  GL_CALL(glDepthFunc, GL_LEQUAL);
  GL_CALL(glEnable, GL_DEPTH_TEST);

  // Update the global uniforms
  GlobalUniform::vertex_data_t global_vertex_data;
  global_vertex_data.camera_matrix = navigation.camera.view_perspective_matrix();
  global_uniform->write(global_vertex_data);
  global_uniform->bind();


  visualization->render();
  point_renderer->render_points();

  global_uniform->unbind();
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

void Viewport::keyPressEvent(QKeyEvent* event)
{
  navigation.keyPressEvent(event);
}

void Viewport::keyReleaseEvent(QKeyEvent* event)
{
  navigation.keyReleaseEvent(event);
}
