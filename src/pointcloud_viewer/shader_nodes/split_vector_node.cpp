#include <pointcloud_viewer/shader_nodes/split_vector_node.hpp>

SplitVectorNode::SplitVectorNode()
{
  vector = std::make_shared<Value>("ivec3(0,0,0)", VALUE_TYPE::IVEC3);
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  z = std::make_shared<Value>("0", VALUE_TYPE::INT);
}

QString SplitVectorNode::caption() const
{
  return "SplitVector";
}

QString SplitVectorNode::name() const
{
  return "SplitVector";
}

uint SplitVectorNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 1;
  case QtNodes::PortType::Out:
    return 3;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString SplitVectorNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::Out:
    if(portIndex == 0)
      return "X";
    else if(portIndex == 1)
      return "Y";
    else if(portIndex == 2)
      return "Z";
    break;
  case QtNodes::PortType::In:
    return "Vector";
  case QtNodes::PortType::None:
    break;
  }

  Q_UNREACHABLE();
  return "";
}

bool SplitVectorNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return portIndex == 0;
  case QtNodes::PortType::Out:
    return portIndex>=0 && portIndex < 3;
  case QtNodes::PortType::None:
    break;
  }

  return false;
}

QtNodes::NodeDataType SplitVectorNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void SplitVectorNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  if(nodeData == nullptr)
    vector = std::make_shared<Value>("ivec3(0,0,0)", VALUE_TYPE::IVEC3);
  else
    vector = std::dynamic_pointer_cast<Value>(nodeData);

  const value_type_t scalar_type = to_scalar(vector->value_type);

  if(is_vector(vector->value_type))
  {
    x = std::make_shared<Value>(QString("%0.x").arg(vector->expression), scalar_type);
    y = std::make_shared<Value>(QString("%0.y").arg(vector->expression), scalar_type);
    z = std::make_shared<Value>(QString("%0.z").arg(vector->expression), scalar_type);
  }else
  {
    x = vector;
    y = vector;
    z = vector;
  }

  dataUpdated(0);
  dataUpdated(1);
  dataUpdated(2);
}

std::shared_ptr<QtNodes::NodeData> SplitVectorNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0 || portIndex == 1 || portIndex == 2);

  if(portIndex == 0)
    return x;
  else if(portIndex == 1)
    return y;
  else
    return z;
}

QWidget* SplitVectorNode::embeddedWidget()
{
  return nullptr;
}
