#ifndef POINTCLOUDVIEWER_SHADER_NODES_COLOR_CLASS_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_COLOR_CLASS_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QMap>

class SwitchNode final : public QtNodes::NodeDataModel
{
public:
  SwitchNode();

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
  constexpr static const int N = 10;
  constexpr static const int index_condition = 0;
  constexpr static const int index_values[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  constexpr static const int index_default = 11;

  std::shared_ptr<Value> conditionInput;
  std::shared_ptr<Value> valuesInput[N];
  std::shared_ptr<Value> defaultValue;
  std::shared_ptr<Value> output;

  int all_classes[N];

  QWidget* _root_widget;

  void update_result();
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_COLOR_CLASS_NODE_HPP_
