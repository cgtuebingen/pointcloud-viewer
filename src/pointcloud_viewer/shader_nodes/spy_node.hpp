#ifndef POINTCLOUDVIEWER_SHADER_NODES_SPY_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_SPY_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>

class SpyNode final : public QtNodes::NodeDataModel
{
public:
  SpyNode();

  QString caption() const override{return "Spy";}
  QString name() const override{return "Spy";}
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  QLabel* _label_expression;
  QLabel* _label_type;
  QWidget* _root;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_SPY_NODE_HPP_
