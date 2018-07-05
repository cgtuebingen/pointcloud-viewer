#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <renderer/gl450/declarations.hpp>
#include <pointcloud_viewer/declarations.hpp>
#include <pointcloud_viewer/camera.hpp>
#include <pointcloud_viewer/navigation.hpp>
#include <pointcloud/pointcloud.hpp>
#include <pointcloud/pointcloud.hpp>

#include <QOpenGLWidget>
#include <functional>
#include <unordered_map>

/*
The viewport is owning the opengl context and delegating the point rendering to
the renderer.
*/

enum class point_cloud_handle_t : size_t
{
  INVALID = std::numeric_limits<size_t>::max(),
};

class Viewport final : public QOpenGLWidget
{
  Q_OBJECT
  Q_PROPERTY(int backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
  Q_PROPERTY(float pointSize READ pointSize WRITE setPointSize NOTIFY pointSizeChanged)
public:
  Navigation navigation;
  bool enable_preview = true;

  Viewport();
  ~Viewport() override;

  void set_camera_frame(const frame_t& frame);

  void unload_all_point_clouds();
  point_cloud_handle_t load_point_cloud(PointCloud&& point_cloud);

  void render_points(frame_t camera_frame, float aspect, std::function<void()> additional_rendering) const;

  int backgroundColor() const;
  float pointSize() const;

public slots:
  void setBackgroundColor(int backgroundColor);
  void setPointSize(float pointSize);

signals:
  void frame_rendered(double duration);

  void backgroundColorChanged(int backgroundColor);
  void pointSizeChanged(float pointSize);

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;

private:
  typedef renderer::gl450::PointRenderer PointRenderer;
  typedef renderer::gl450::GlobalUniform GlobalUniform;

  PointRenderer* point_renderer = nullptr;
  GlobalUniform* global_uniform = nullptr;

  Visualization* visualization;

  std::unordered_map<size_t, PointCloud> point_clouds;
  size_t next_handle = 0;
  int m_backgroundColor = 0;
  float m_pointSize = 1;
};



#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
