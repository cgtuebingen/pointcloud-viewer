#include <pointcloud_viewer/shader_nodes/rgb_node.hpp>

RgbNode::RgbNode()
{
  _rgb_widget = new RgbEdit;
  connect(_rgb_widget, &RgbEdit::colorChanged, this, [this](){dataUpdated(0);});
}

QJsonObject RgbNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["rgb"] = _rgb_widget->text();

  return jsonObject;
}

void RgbNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  _rgb_widget->setText(jsonObject["rgb"].toString());
}

QString RgbNode::caption() const
{
  return "Rgb";
}

QString RgbNode::name() const
{
  return "Rgb";
}

uint RgbNode::nPorts(QtNodes::PortType portType) const
{
  if(portType == QtNodes::PortType::Out)
    return 1;
  else
    return 0;
}

QtNodes::NodeDataType RgbNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void RgbNode::setInData(std::shared_ptr<QtNodes::NodeData>, QtNodes::PortIndex)
{
}

std::shared_ptr<QtNodes::NodeData> RgbNode::outData(QtNodes::PortIndex)
{
  return std::make_shared<Value>(QString("uvec3(%0, %1, %2)").arg(_rgb_widget->red()).arg(_rgb_widget->green()).arg(_rgb_widget->blue()), VALUE_TYPE::UVEC3);
}

QWidget* RgbNode::embeddedWidget()
{
  return _rgb_widget;
}
