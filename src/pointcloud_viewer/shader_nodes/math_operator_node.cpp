#include <pointcloud_viewer/shader_nodes/math_operator_node.hpp>

MathOperatorNode::MathOperatorNode()
{
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  result = std::make_shared<Value>("0", VALUE_TYPE::INT);

  _combobox_op = new QComboBox;
  _combobox_op->addItem("add (x + y)", "+");
  _combobox_op->addItem("subtract (x - y)", "-");
  _combobox_op->addItem("multiplicate (x * y)", "*");
  _combobox_op->addItem("divide (x / y)", "/");
  _combobox_op->addItem("bitwise and (x & y)", "&");
  _combobox_op->addItem("bitwise or (x | y)", "|");
  _combobox_op->addItem("bitwise xor (x ^ y)", "^");

  connect(_combobox_op, &QComboBox::currentTextChanged, this, &MathOperatorNode::update_operator);
  update_operator();
}

QJsonObject MathOperatorNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["operator"] = _combobox_op->currentData().toString();

  return jsonObject;
}

void MathOperatorNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  set_operator(jsonObject["operator"].toString());
}

void MathOperatorNode::set_operator(QString op)
{
  int index = _combobox_op->findData(op);

  if(index >= 0)
    _combobox_op->setCurrentIndex(index);
}

QString MathOperatorNode::caption() const
{
  return "Operator";
}

QString MathOperatorNode::name() const
{
  return "Operator";
}

uint MathOperatorNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 2;
  case QtNodes::PortType::Out:
    return 1;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString MathOperatorNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    if(portIndex == 0)
      return "X";
    else if(portIndex == 1)
      return "Y";
    break;
  case QtNodes::PortType::Out:
    return "Result";
  case QtNodes::PortType::None:
    break;
  }

  Q_UNREACHABLE();
  return "";
}

bool MathOperatorNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return portIndex>=0 && portIndex < 2;
  case QtNodes::PortType::Out:
    return false;
  case QtNodes::PortType::None:
    break;
  }

  return false;
}

QtNodes::NodeDataType MathOperatorNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void MathOperatorNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0 || portIndex == 1 || portIndex == 2);

  if(portIndex == 0)
  {
    if(nodeData == nullptr)
      x = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      x = std::dynamic_pointer_cast<Value>(nodeData);
  }else
  {
    if(nodeData == nullptr)
      y = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      y = std::dynamic_pointer_cast<Value>(nodeData);
  }

  update_result();
}

std::shared_ptr<QtNodes::NodeData> MathOperatorNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  return result;
}

QWidget* MathOperatorNode::embeddedWidget()
{
  return _combobox_op;
}

void MathOperatorNode::update_result()
{
  value_type_t result_type = ::result_type(x->value_type, y->value_type);
  result = std::make_shared<Value>(QString("(%0 %2 %1)").arg(x->expression).arg(y->expression).arg(operator_symbol), result_type);
  dataUpdated(0);
}

void MathOperatorNode::update_operator()
{
  operator_symbol = _combobox_op->currentData().toString();
  update_result();
}
