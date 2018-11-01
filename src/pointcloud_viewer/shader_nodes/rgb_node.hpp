#ifndef POINTCLOUDVIEWER_SHADER_NODES_RGB_NODE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_RGB_NODE_HPP_

#include <pointcloud_viewer/shader_nodes/value.hpp>
#include <pointcloud_viewer/widgets/rgb_edit.hpp>

#include <nodes/NodeDataModel>

#include <QLabel>
#include <QMap>
#include <QComboBox>

class RgbNode final : public QtNodes::NodeDataModel
{
public:
  RgbNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & jsonObject) override;

  QString caption() const override;
  QString name() const override;
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType, QtNodes::PortIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  RgbEdit* _rgb_widget;
};

#endif // POINTCLOUDVIEWER_SHADER_NODES_RGB_NODE_HPP_
