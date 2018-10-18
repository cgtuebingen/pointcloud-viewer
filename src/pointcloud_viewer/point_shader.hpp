#ifndef POINT_SHADER_HPP
#define POINT_SHADER_HPP

#include <pointcloud/pointcloud.hpp>

#include <QSharedPointer>
#include <memory>

namespace QtNodes
{

struct DataModelRegistry;

} // namespace QtNodes

class PointShader
{
public:
  struct property_t
  {
    QString name;
    data_type::base_type_t type;
  };

  PointShader();
  ~PointShader();

  PointShader(const PointShader& pointShader);
  PointShader(PointShader&& pointShader);

  PointShader& operator=(const PointShader& pointShader);
  PointShader& operator=(PointShader&& pointShader);

  QString name() const;
  QVector<property_t> properties() const;
  bool contains_property(const QString& name) const;

  static PointShader autogenerate(const QSharedPointer<PointCloud>& pointcloud);
  PointShader clone() const;

  static PointShader import_from_file(QString filename);
  void export_to_file(QString filename) const;

  bool edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud);

  void apply_shader(const QSharedPointer<PointCloud>& currentPointcloud) const;
  QString shader_code_glsl450(const QSharedPointer<PointCloud>& currentPointcloud) const;

private:
  class Implementation;

  QSharedPointer<Implementation>  _implementation;

  explicit PointShader(const QSharedPointer<Implementation>& implementation);

  std::shared_ptr<QtNodes::DataModelRegistry> qt_nodes_model_registry(const QSharedPointer<PointCloud>& pointcloud) const;
};

class PointShader::Implementation
{
public:
  QString name;

  QVector<property_t> properties;

  QByteArray nodes;

  QSharedPointer<PointShader::Implementation> clone() const;
};

Q_DECLARE_METATYPE(PointShader);

#endif // POINT_SHADER_HPP
