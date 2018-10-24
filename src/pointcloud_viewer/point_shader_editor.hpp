#ifndef POINT_SHADER_HPP
#define POINT_SHADER_HPP

#include <pointcloud/pointcloud.hpp>

#include <QSharedPointer>
#include <QWidget>
#include <memory>

namespace QtNodes
{

struct DataModelRegistry;

} // namespace QtNodes

class Viewport;

class PointShaderEditor : public QWidget
{
public:
  PointShaderEditor();
  ~PointShaderEditor();

  void unload_all_point_clouds();
  void load_point_cloud(QSharedPointer<PointCloud> point_cloud);

  PointCloud::Shader autogenerate() const;

private:
  QSharedPointer<PointCloud>  _pointCloud;

  std::shared_ptr<QtNodes::DataModelRegistry> qt_nodes_model_registry(const QSharedPointer<PointCloud>& pointcloud) const;
};

#endif // POINT_SHADER_HPP
