#include <pointcloud_viewer/point_shader.hpp>
#include <core_library/print.hpp>

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

QVector<PointShader::property_t> PointShader::properties() const
{
  return _implementation->properties;
}

bool PointShader::contains_property(const QString& name) const
{
  for(const property_t& property : _implementation->properties)
    if(property.name == name)
      return true;
  return false;
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
  PRINT(filename.toStdString());
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
  implementation->properties = this->properties;
  implementation->nodes = this->nodes;

  return implementation;
}
