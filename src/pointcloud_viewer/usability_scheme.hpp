#ifndef POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_
#define POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_

#include <QMouseEvent>

class UsabilityScheme final : public QObject
{
public:
  class Implementation;

  Implementation* implementation;

  ~UsabilityScheme();

  void wheelEvent(QWheelEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void focusOutEvent(QFocusEvent* event);
};

class UsabilityScheme::Implementation : public QObject
{
public:
  virtual ~Implementation() = default;

  virtual void wheelEvent(QWheelEvent* event) = 0;
  virtual void mouseMoveEvent(QMouseEvent* event) = 0;
  virtual void mousePressEvent(QMouseEvent* event) = 0;
  virtual void mouseReleaseEvent(QMouseEvent* event) = 0;
  virtual void keyPressEvent(QKeyEvent* event) = 0;
  virtual void keyReleaseEvent(QKeyEvent* event) = 0;
  virtual void focusOutEvent(QFocusEvent* event) = 0;
};

#endif // POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_
