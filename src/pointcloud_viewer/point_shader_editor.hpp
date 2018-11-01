#ifndef POINT_SHADER_HPP
#define POINT_SHADER_HPP

#include <pointcloud/pointcloud.hpp>

#include <QDialogButtonBox>
#include <QSharedPointer>
#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>

#include <memory>

namespace QtNodes
{

struct DataModelRegistry;
struct FlowScene;
struct FlowView;

} // namespace QtNodes

class MainWindow;

class PointShaderEditor : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(bool isPointCloudLoaded READ isPointCloudLoaded NOTIFY isPointCloudLoadedChanged)
  Q_PROPERTY(bool isReadOnly READ isReadOnly WRITE setIsReadOnly NOTIFY isReadOnlyChanged)
  Q_PROPERTY(QString shaderName READ shaderName WRITE setShaderName NOTIFY shaderNameChanged)
public:
  PointShaderEditor(MainWindow* mainWindow);
  ~PointShaderEditor();

  void unload_all_point_clouds();
  void load_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void unload_shader();
  void load_shader(PointCloud::Shader shader);

  static PointCloud::Shader autogenerate(const PointCloud* pointcloud);

  bool isPointCloudLoaded() const;
  bool isReadOnly() const;

  QString shaderName() const;

public slots:
  void setIsReadOnly(bool isReadOnly);
  void setShaderName(QString shaderName);

  void applyShader();

signals:
  void isPointCloudLoadedChanged(bool isPointCloudLoaded);

  void isReadOnlyChanged(bool isReadOnly);

  void shader_applied(bool coordinates_changed, bool colors_changed);

  void shaderNameChanged(QString shaderName);

private:
  friend QSet<QString> find_used_properties(const PointCloud* pointcloud);
  friend PointCloud::Shader generate_code_from_shader(const PointCloud* pointcloud);

  MainWindow& mainWindow;

  QSharedPointer<PointCloud>  _pointCloud;
  QtNodes::FlowScene* flowScene = nullptr;

  QtNodes::FlowView* flowView = nullptr;
  QtNodes::FlowScene* fallbackFlowScene = nullptr;
  QAction* importShader_action = nullptr;
  QAction* exportShader_action = nullptr;

  QLabel* readonlyNotificationBar;
  QLineEdit* shaderName_Editor;

  static std::shared_ptr<QtNodes::DataModelRegistry> qt_nodes_model_registry(const PointCloud* pointcloud);

  bool m_isReadOnly = false;

  QString m_shaderName;

private slots:
  void closeEditor();
  void importShader();
  void exportShader();
};

QSet<QString> find_used_properties(const PointCloud* pointcloud);
QSet<QString> find_used_properties(QtNodes::FlowScene* flowScene);
PointCloud::Shader generate_code_from_shader(const PointCloud* pointcloud);
PointCloud::Shader generate_code_from_shader(QtNodes::FlowScene* flowScene, PointCloud::Shader shader);


#endif // POINT_SHADER_HPP
