#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <glad/glad.h>
#include <render_system/point_renderer.hpp>

#include <QOpenGLWidget>

class Viewport final : public QOpenGLWidget
{
  Q_OBJECT
public:
  Viewport();
  ~Viewport() override;

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

public:
  render_system::PointRenderer* point_renderer = nullptr;
};

#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
