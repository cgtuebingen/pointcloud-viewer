#include "point_shader.hpp"

class PointShader::Implementation
{
public:
  QString name;
  QSharedPointer<PointShader::Implementation> clone() const;
};

PointShader::PointShader()
  : _implementation(autogenerate(QSharedPointer<PointCloud>())._implementation)
{
}

PointShader::~PointShader()
{
}

PointShader::PointShader(const PointShader& pointShader)
  : _implementation(pointShader._implementation)
{
}

PointShader::PointShader(PointShader&& pointShader)
  : _implementation(std::move(pointShader._implementation))
{
}

PointShader& PointShader::operator=(const PointShader& pointShader)
{
  _implementation = pointShader._implementation;
  return *this;
}

PointShader& PointShader::operator=(PointShader&& pointShader)
{
  _implementation.swap(pointShader._implementation);
  return *this;
}

QString PointShader::name() const
{
  return _implementation->name;
}

PointShader PointShader::autogenerate(const QSharedPointer<PointCloud>& pointcloud)
{
  QSharedPointer<Implementation> implementation(new Implementation);

  implementation->name = "auto";

  // TODO
  return PointShader(implementation);
}

PointShader PointShader::clone() const
{
  return PointShader(_implementation->clone());
}

PointShader PointShader::import_from_file(QString filename)
{
  // TODO
  return PointShader();
}

void PointShader::export_to_file(QString filename) const
{
  // TODO
}

void PointShader::edit(QWidget* parent)
{
  // TODO
}

PointShader::PointShader(const QSharedPointer<PointShader::Implementation>& implementation)
  : _implementation(implementation)
{
}

QSharedPointer<PointShader::Implementation> PointShader::Implementation::clone() const
{
  QSharedPointer<Implementation> implementation(new Implementation);

  implementation->name = this->name + " (Copy)";

  return implementation;
}
