#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <renderer/gl450/declarations.hpp>
#include <pointcloud_viewer/declarations.hpp>
#include <pointcloud_viewer/camera.hpp>
#include <pointcloud_viewer/navigation.hpp>

#include <QOpenGLWidget>

/*
The viewport is owning the opengl context and delegating the point rendering to
the renderer.
*/

class Viewport final : public QOpenGLWidget
{
  Q_OBJECT
public:
  Navigation navigation;

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

  PointRenderer* point_renderer = nullptr;
  GlobalUniform* global_uniform = nullptr;

  Visualization* visualization;
};





#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
