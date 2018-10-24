#ifndef POINTCLOUDVIEWER_SHADER_NODES_PROPERTY_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_PROPERTY_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>
#include <QMap>
#include <QComboBox>

class PropertyNode final : public QtNodes::NodeDataModel
{
public:
  PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_type, QPixmap warning_icons);

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_property(const QString& name);
  QString property_name() const;

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private slots:
  void changedProperty(QString name);

private:
  QMap<QString, property_type_t> property_base_types;
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _property = std::make_shared<Value>();

  QWidget* _hbox_widget;
  QLabel* _warning_widget;
  QComboBox* _combobox;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_PROPERTY_NODE_HPP_
