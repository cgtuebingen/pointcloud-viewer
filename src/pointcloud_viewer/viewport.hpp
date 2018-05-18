#ifndef POINTCLOUDVIEWER_VIEWPORT_HPP_
#define POINTCLOUDVIEWER_VIEWPORT_HPP_

#include <QOpenGLWidget>

class Viewport : public QOpenGLWidget
{
  Q_OBJECT
public:
  Viewport();
  ~Viewport();
};

#endif // POINTCLOUDVIEWER_VIEWPORT_HPP_
