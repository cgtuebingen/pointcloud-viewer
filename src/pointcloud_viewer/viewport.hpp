#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <renderer/gl450/declarations.hpp>

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

private:
  typedef render_system::gl450::PointRenderer PointRenderer;

  class Visualization;

  PointRenderer* point_renderer = nullptr;

  Visualization* visualization;
};

#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
