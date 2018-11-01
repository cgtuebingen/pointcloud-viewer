#include <pointcloud_viewer/shader_nodes/mix_node.hpp>

MixNode::MixNode()
{
}

QString MixNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  if(portType == QtNodes::PortType::In)
  {
    switch(portIndex)
    {
    case 0:
      return "x";
    case 1:
      return "y";
    case 2:
      return "alpha";
    }
  }

  return QString();
}

bool MixNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex) const
{
  return portType == QtNodes::PortType::In;
}

QString MixNode::caption() const
{
  return "Mix";
}

QString MixNode::name() const
{
  return "Mix";
}

uint MixNode::nPorts(QtNodes::PortType portType) const
{
  if(portType == QtNodes::PortType::In)
    return 3;
  else
    return 1;
}

QtNodes::NodeDataType MixNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void MixNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port)
{
  std::shared_ptr<Value> value = std::dynamic_pointer_cast<Value>(nodeData);

  arguments[port] = value;

  dataUpdated(0);
}

std::shared_ptr<QtNodes::NodeData> MixNode::outData(QtNodes::PortIndex)
{
  bool any_input_is_vector = false;
  bool any_input_is_double = false;
  bool any_input_is_float = false;

  for(int i=0; i<3; ++i)
  {
    if(arguments[i] == nullptr)
      arguments[i] = std::make_shared<Value>("0", VALUE_TYPE::FLOAT);
    else
    {
      any_input_is_vector = any_input_is_vector || is_vector(arguments[i]->value_type);
      any_input_is_double = any_input_is_double || to_scalar(arguments[i]->value_type)==VALUE_TYPE::DOUBLE;
      any_input_is_float = any_input_is_double || to_scalar(arguments[i]->value_type)==VALUE_TYPE::FLOAT;
    }
  }

  value_type_t type;
  if(any_input_is_vector)
  {
    if(any_input_is_double)
      type = VALUE_TYPE::DVEC3;
    else
      type = VALUE_TYPE::VEC3;
  }else
  {
    if(any_input_is_double)
      type = VALUE_TYPE::DOUBLE;
    else
      type = VALUE_TYPE::FLOAT;
  }

  QString expression = "mix(";
  expression += Value::cast(arguments[0], type)->expression;
  expression += ", ";
  expression += Value::cast(arguments[1], type)->expression;
  expression += ", ";
  expression += Value::cast(arguments[2], type)->expression;
  expression += ")";

  return std::make_shared<Value>(expression, type);
}

QWidget*MixNode::embeddedWidget()
{
  return nullptr;
}
