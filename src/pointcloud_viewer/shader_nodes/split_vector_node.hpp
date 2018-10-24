#ifndef POINTCLOUDVIEWER_SHADER_NODES_SPLIT_VECTOR_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_SPLIT_VECTOR_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

class SplitVectorNode final : public QtNodes::NodeDataModel
{
public:
  SplitVectorNode();

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> vector;
  std::shared_ptr<Value> x, y, z;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_SPLIT_VECTOR_NODE_HPP_
