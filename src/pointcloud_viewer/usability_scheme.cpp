#include <pointcloud_viewer/usability_scheme.hpp>


UsabilityScheme::~UsabilityScheme()
{

}

void UsabilityScheme::wheelEvent(QWheelEvent* event)
{
  implementation->wheelEvent(event);
}

void UsabilityScheme::mouseMoveEvent(QMouseEvent* event)
{

  implementation->mouseMoveEvent(event);
}

void UsabilityScheme::mousePressEvent(QMouseEvent* event)
{
  implementation->mousePressEvent(event);
}

void UsabilityScheme::mouseReleaseEvent(QMouseEvent* event)
{
  implementation->mouseReleaseEvent(event);
}

void UsabilityScheme::keyPressEvent(QKeyEvent* event)
{
  implementation->keyPressEvent(event);
}

void UsabilityScheme::keyReleaseEvent(QKeyEvent* event)
{
  implementation->keyReleaseEvent(event);
}

void UsabilityScheme::focusOutEvent(QFocusEvent* event)
{
  implementation->focusOutEvent(event);
}
