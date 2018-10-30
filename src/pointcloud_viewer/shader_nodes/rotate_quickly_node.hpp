#ifndef POINTCLOUDVIEWER_SHADER_NODES_ROTATE_QUICKLY_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_ROTATE_QUICKLY_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>
#include <QMap>
#include <QComboBox>

class RotateQuicklyNode final : public QtNodes::NodeDataModel
{
public:
  RotateQuicklyNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_axis(QString op);

  QString caption() const override{return "RotateQuickly";}
  QString name() const override{return "RotateQuickly";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> vector;
  std::shared_ptr<Value> rotated;
  QComboBox* _axis;

  void update_result();
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_ROTATE_QUICKLY_NODE_HPP_
