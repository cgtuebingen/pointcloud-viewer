#ifndef POINT_SHADER_HPP
#define POINT_SHADER_HPP

#include <pointcloud/pointcloud.hpp>

#include <QDialogButtonBox>
#include <QSharedPointer>
#include <QWidget>
#include <memory>

namespace QtNodes
{

struct DataModelRegistry;
struct FlowScene;
struct FlowView;

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

  void setShaderEditableEnabled(bool enabled);

private:
  friend
  QSet<QString> find_used_properties(const QSharedPointer<PointCloud>& pointcloud);

  QSharedPointer<PointCloud>  _pointCloud;
  QtNodes::FlowView* flowView = nullptr;
  QtNodes::FlowScene* flowScene = nullptr;
  QtNodes::FlowScene* fallbackFlowScene = nullptr;
  QDialogButtonBox* buttonGroup = nullptr;

  static std::shared_ptr<QtNodes::DataModelRegistry> qt_nodes_model_registry(const QSharedPointer<PointCloud>& pointcloud);
};

QSet<QString> find_used_properties(const QSharedPointer<PointCloud>& pointcloud);

#endif // POINT_SHADER_HPP
