#ifndef POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_
#define POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_

#include <QMouseEvent>
#include <QSharedPointer>
#include <QVector>

// Different Usability schemes on how to navigate the 3d space

class UsabilityScheme final : public QObject
{
public:
  enum scheme_t
  {
    BLENDER = 0,
    MESHLAB = 1,
  };

  UsabilityScheme();
  ~UsabilityScheme();

  void enableBlenderScheme();
  void enableMeshlabScheme();

  void enableScheme(scheme_t scheme);

  void wheelEvent(QWheelEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void focusOutEvent(QFocusEvent* event);

private:
  class Implementation;

  Implementation* _implementation = nullptr;

  QMap<scheme_t, QSharedPointer<Implementation>> implementations;
};

class UsabilityScheme::Implementation
{
public:
  class BlenderScheme;
  class MeshLabScheme;

  virtual ~Implementation() = default;

  virtual void on_enable() = 0;
  virtual void on_disable() = 0;

  virtual void wheelEvent(QWheelEvent* event) = 0;
  virtual void mouseMoveEvent(QMouseEvent* event) = 0;
  virtual void mousePressEvent(QMouseEvent* event) = 0;
  virtual void mouseReleaseEvent(QMouseEvent* event) = 0;
  virtual void keyPressEvent(QKeyEvent* event) = 0;
  virtual void keyReleaseEvent(QKeyEvent* event) = 0;
  virtual void focusOutEvent(QFocusEvent* event) = 0;
};
#endif // POINTCLOUDVIEWER_USABILILTY_SCHEME_HPP_
