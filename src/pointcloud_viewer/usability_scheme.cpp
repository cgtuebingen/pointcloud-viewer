#include <pointcloud_viewer/usability_scheme.hpp>


class UsabilityScheme::Implementation::BlenderScheme final : public UsabilityScheme::Implementation
{
public:
  BlenderScheme();

  void on_enable() override;
  void on_disable() override;

  void wheelEvent(QWheelEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;
};

class UsabilityScheme::Implementation::MeshLabScheme final : public UsabilityScheme::Implementation
{
public:
  MeshLabScheme();

  void on_enable() override;
  void on_disable() override;

  void wheelEvent(QWheelEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;
};


UsabilityScheme::UsabilityScheme()
{
  implementations[BLENDER] = QSharedPointer<Implementation>(new Implementation::BlenderScheme);
  implementations[MESHLAB] = QSharedPointer<Implementation>(new Implementation::MeshLabScheme);

  enableBlenderScheme();
}

UsabilityScheme::~UsabilityScheme()
{
  if(_implementation != nullptr)
    _implementation->on_disable();
}

void UsabilityScheme::enableBlenderScheme()
{
  enableScheme(BLENDER);
}

void UsabilityScheme::enableMeshlabScheme()
{
  enableScheme(MESHLAB);
}

void UsabilityScheme::enableScheme(scheme_t scheme)
{
  if(_implementation != nullptr)
    _implementation->on_disable();

  _implementation = implementations.value(scheme, implementations.value(BLENDER)).data();

  if(_implementation != nullptr)
    _implementation->on_enable();
}

void UsabilityScheme::wheelEvent(QWheelEvent* event)
{
  _implementation->wheelEvent(event);
}

void UsabilityScheme::mouseMoveEvent(QMouseEvent* event)
{

  _implementation->mouseMoveEvent(event);
}

void UsabilityScheme::mousePressEvent(QMouseEvent* event)
{
  _implementation->mousePressEvent(event);
}

void UsabilityScheme::mouseReleaseEvent(QMouseEvent* event)
{
  _implementation->mouseReleaseEvent(event);
}

void UsabilityScheme::keyPressEvent(QKeyEvent* event)
{
  _implementation->keyPressEvent(event);
}

void UsabilityScheme::keyReleaseEvent(QKeyEvent* event)
{
  _implementation->keyReleaseEvent(event);
}

void UsabilityScheme::focusOutEvent(QFocusEvent* event)
{
  _implementation->focusOutEvent(event);
}

// ==== Blender ====

UsabilityScheme::Implementation::BlenderScheme::BlenderScheme()
{
}

void UsabilityScheme::Implementation::BlenderScheme::on_enable()
{

}

void UsabilityScheme::Implementation::BlenderScheme::on_disable()
{

}

void UsabilityScheme::Implementation::BlenderScheme::wheelEvent(QWheelEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::mouseMoveEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::mousePressEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::mouseReleaseEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::keyPressEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::keyReleaseEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::BlenderScheme::focusOutEvent(QFocusEvent* event)
{

}

// ==== Meshlab ====

UsabilityScheme::Implementation::MeshLabScheme::MeshLabScheme()
{
}

void UsabilityScheme::Implementation::MeshLabScheme::on_enable()
{

}

void UsabilityScheme::Implementation::MeshLabScheme::on_disable()
{

}

void UsabilityScheme::Implementation::MeshLabScheme::wheelEvent(QWheelEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mouseMoveEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mousePressEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::mouseReleaseEvent(QMouseEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::keyPressEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::keyReleaseEvent(QKeyEvent* event)
{

}

void UsabilityScheme::Implementation::MeshLabScheme::focusOutEvent(QFocusEvent* event)
{

}
