#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <renderer/gl450/declarations.hpp>
#include <pointcloud_viewer/declarations.hpp>
#include <pointcloud_viewer/camera.hpp>

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

private:
  typedef renderer::gl450::PointRenderer PointRenderer;

  float aspect_ratio = 1.f;

  PointRenderer* point_renderer = nullptr;

  Visualization* visualization;
};

#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
