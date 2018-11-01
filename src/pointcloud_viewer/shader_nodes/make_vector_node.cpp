#include <pointcloud_viewer/shader_nodes/make_vector_node.hpp>

MakeVectorNode::MakeVectorNode()
{
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  z = std::make_shared<Value>("0", VALUE_TYPE::INT);
  vector = std::make_shared<Value>("ivec3(0,0,0)", VALUE_TYPE::IVEC3);
}

QString MakeVectorNode::caption() const
{
  return "MakeVector";
}

QString MakeVectorNode::name() const
{
  return "MakeVector";
}

uint MakeVectorNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 3;
  case QtNodes::PortType::Out:
    return 1;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString MakeVectorNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    if(portIndex == 0)
      return "X";
    else if(portIndex == 1)
      return "Y";
    else if(portIndex == 2)
      return "Z";
    break;
  case QtNodes::PortType::Out:
    return "Vector";
  case QtNodes::PortType::None:
    break;
  }

  Q_UNREACHABLE();
  return "";
}

bool MakeVectorNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return portIndex>=0 && portIndex < 3;
  case QtNodes::PortType::Out:
    return portIndex == 0;
  case QtNodes::PortType::None:
    break;
  }

  return false;
}

QtNodes::NodeDataType MakeVectorNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void MakeVectorNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0 || portIndex == 1 || portIndex == 2);

  if(portIndex == 0)
  {
    if(nodeData == nullptr)
      x = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      x = std::dynamic_pointer_cast<Value>(nodeData);
  }else if(portIndex == 1)
  {
    if(nodeData == nullptr)
      y = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      y = std::dynamic_pointer_cast<Value>(nodeData);
  }else
  {
    if(nodeData == nullptr)
      z = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      z = std::dynamic_pointer_cast<Value>(nodeData);
  }

  x = Value::cast(x, to_scalar(x->value_type));
  y = Value::cast(y, to_scalar(y->value_type));
  z = Value::cast(z, to_scalar(z->value_type));

  value_type_t vector_type = to_vector(result_type(x->value_type, y->value_type, z->value_type));
  vector = std::make_shared<Value>(QString("%0(%1, %2, %3)").arg(format(vector_type)).arg(x->expression).arg(y->expression).arg(z->expression), vector_type);
  dataUpdated(0);
}

std::shared_ptr<QtNodes::NodeData> MakeVectorNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  return vector;
}

QWidget* MakeVectorNode::embeddedWidget()
{
  return nullptr;
}
