#ifndef POINTCLOUDVIEWER_SHADER_NODES_MATH_OPERATOR_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_MATH_OPERATOR_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QComboBox>

class MathOperatorNode final : public QtNodes::NodeDataModel
{
public:
  MathOperatorNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_operator(QString op);

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> x, y;
  std::shared_ptr<Value> result;
  QString operator_symbol;
  QComboBox* _combobox_op;

  void update_result();
  void update_operator();
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_MATH_OPERATOR_NODE_HPP_
