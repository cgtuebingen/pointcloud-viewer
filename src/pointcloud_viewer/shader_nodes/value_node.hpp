#ifndef POINTCLOUDVIEWER_SHADER_NODES_VALUE_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_VALUE_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLineEdit>
#include <QComboBox>

class ValueNode final : public QtNodes::NodeDataModel
{
public:
  ValueNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_value(QString value, value_type_t value_type);

  QString caption() const override{return "Value";}
  QString name() const override{return "Value";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> value;
  QString operator_symbol;
  QLineEdit* _content_box;
  QComboBox* _combobox_type;
  QWidget* _root;

  void update_value_expression();
  void update_value_type();
};


#endif // POINTCLOUDVIEWER_SHADER_NODES_VALUE_NODE_HPP_
