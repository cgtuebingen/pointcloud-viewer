#ifndef POINTCLOUDVIEWER_SHADER_NODES_MIX_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_MIX_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>
#include <QMap>
#include <QComboBox>

class MixNode final : public QtNodes::NodeDataModel
{
public:
  MixNode();

  QString portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;
  bool portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex) const override;

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> arguments[3];
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_RGB_NODE_HPP_
