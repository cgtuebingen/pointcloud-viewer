#include <pointcloud_viewer/shader_nodes/output_node.hpp>

OutputNode::OutputNode()
{
  reset_coordinate();
  reset_color();
}

QString OutputNode::caption() const
{
  return "Output";
}

QString OutputNode::name() const
{
  return "Output";
}

uint OutputNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 2;
  case QtNodes::PortType::Out:
    return 0;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString OutputNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::In);
  Q_ASSERT(portIndex == 0 || portIndex == 1);

  if(portIndex == 0)
    return "Coordinate";
  else
    return "Color";
}

bool OutputNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::In);
  Q_ASSERT(portIndex == 0 || portIndex == 1);

  return true;
}

QtNodes::NodeDataType OutputNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::In);
  Q_ASSERT(portIndex == 0 || portIndex == 1);
  return Value().type();
}

void OutputNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0 || portIndex == 1);

  if(nodeData == nullptr)
  {
    if(portIndex == 0)
      reset_coordinate();
    else
      reset_color();
  }else
  {
    auto value = std::dynamic_pointer_cast<Value>(nodeData);

    if(portIndex == 0)
      coordinate = Value::cast(value, VALUE_TYPE::VEC3);
    else
      color = Value::cast(value, VALUE_TYPE::UVEC3);
  }
}

std::shared_ptr<QtNodes::NodeData> OutputNode::outData(QtNodes::PortIndex portIndex)
{
  Q_UNREACHABLE();
  Q_UNUSED(portIndex);
}

QWidget* OutputNode::embeddedWidget()
{
  return nullptr;
}

void OutputNode::reset_coordinate()
{
  coordinate.reset();
}

void OutputNode::reset_color()
{
  color.reset();
}
