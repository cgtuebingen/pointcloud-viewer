#ifndef POINT_SHADER_HPP
#define POINT_SHADER_HPP

#include <QSharedPointer>

#include <pointcloud/pointcloud.hpp>

class PointShader
{
public:
  PointShader();
  ~PointShader();

  PointShader(const PointShader& pointShader);
  PointShader(PointShader&& pointShader);

  PointShader& operator=(const PointShader& pointShader);
  PointShader& operator=(PointShader&& pointShader);

  QString name() const;

  static PointShader autogenerate(const QSharedPointer<PointCloud>& pointcloud);
  PointShader clone() const;

  static PointShader import_from_file(QString filename);
  void export_to_file(QString filename) const;

  void edit(QWidget* parent);

private:
  class Implementation;

  QSharedPointer<Implementation>  _implementation;

  explicit PointShader(const QSharedPointer<Implementation>& implementation);
};

Q_DECLARE_METATYPE(PointShader);

#endif // POINT_SHADER_HPP
