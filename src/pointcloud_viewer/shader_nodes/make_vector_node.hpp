#ifndef POINTCLOUDVIEWER_SHADER_NODES_MAKE_VECTOR_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_MAKE_VECTOR_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

class MakeVectorNode final : public QtNodes::NodeDataModel
{
public:
  MakeVectorNode();

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
  std::shared_ptr<Value> x, y, z;
  std::shared_ptr<Value> vector;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_MAKE_VECTOR_NODE_HPP_
