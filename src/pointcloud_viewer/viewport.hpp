#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <renderer/gl450/declarations.hpp>
#include <pointcloud_viewer/declarations.hpp>
#include <pointcloud_viewer/camera.hpp>
#include <pointcloud_viewer/navigation.hpp>

#include <QOpenGLWidget>


class Viewport final : public QOpenGLWidget
{
  Q_OBJECT
public:
  Camera camera;

  Viewport();
  ~Viewport() override;

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

  Navigation navigation;
  glm::vec3 key_force = glm::vec3(0.f);
  glm::ivec2 last_mouse_pos;

  PointRenderer* point_renderer = nullptr;
  GlobalUniform* global_uniform = nullptr;

  Visualization* visualization;
};



#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
