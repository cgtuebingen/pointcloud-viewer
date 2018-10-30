#ifndef POINTCLOUDVIEWER_SHADER_NODES_OUTPUT_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_OUTPUT_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

class OutputNode final : public QtNodes::NodeDataModel
{
public:
  OutputNode();

  std::shared_ptr<Value> coordinate;
  std::shared_ptr<Value> color;

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
  void reset_coordinate();
  void reset_color();
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_OUTPUT_NODE_HPP_
