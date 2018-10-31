#include <pointcloud_viewer/shader_nodes/rotate_quickly_node.hpp>

RotateQuicklyNode::RotateQuicklyNode()
{
  vector = std::make_shared<Value>("vec3(0)", VALUE_TYPE::VEC3);
  rotated = std::make_shared<Value>("vec3(0)", VALUE_TYPE::VEC3);

  _axis = new QComboBox;
  _axis->addItem("+X", "(%0 * mat3("
                       " 1, 0, 0,"
                       " 0, 0,-1,"
                       " 0, 1, 0"
                       "))");
  _axis->addItem("-X", "(%0 * mat3("
                       " 1, 0, 0,"
                       " 0, 0, 1,"
                       " 0,-1, 0"
                       "))");
  _axis->addItem("+Y", "(%0 * mat3("
                       " 0, 0, 1,"
                       " 0, 1, 0,"
                       "-1, 0, 0"
                       "))");
  _axis->addItem("-Y", "(%0 * mat3("
                       " 0, 0,-1,"
                       " 0, 1, 0,"
                       " 1, 0, 0"
                       "))");
  _axis->addItem("+Z", "(%0 * mat3("
                       " 0,-1, 0,"
                       " 1, 0, 0,"
                       " 0, 0, 1"
                       "))");
  _axis->addItem("-Z", "(%0 * mat3("
                       " 0, 1, 0,"
                       "-1, 0, 0,"
                       " 0, 0, 1"
                       "))");


  connect(_axis, &QComboBox::currentTextChanged, this, &RotateQuicklyNode::update_result);
  update_result();
}

QJsonObject RotateQuicklyNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["axis"] = _axis->currentText();

  return jsonObject;
}

void RotateQuicklyNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  set_axis(jsonObject["axis"].toString());
}

void RotateQuicklyNode::set_axis(QString op)
{
  int index = _axis->findText(op);

  if(index >= 0)
    _axis->setCurrentIndex(index);
}

uint RotateQuicklyNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 1;
  case QtNodes::PortType::Out:
    return 1;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString RotateQuicklyNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portIndex == 0);

  switch(portType)
  {
  case QtNodes::PortType::In:
    return "Vector";
  case QtNodes::PortType::Out:
    return "Rotated";
  case QtNodes::PortType::None:
    break;
  }

  Q_UNREACHABLE();
  return "";
}

bool RotateQuicklyNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return portIndex==0;
  case QtNodes::PortType::Out:
    return portIndex==0;
  case QtNodes::PortType::None:
    break;
  }

  return false;
}

QtNodes::NodeDataType RotateQuicklyNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void RotateQuicklyNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0 || portIndex == 1 || portIndex == 2);

  if(portIndex == 0)
  {
    if(nodeData == nullptr)
      vector = std::make_shared<Value>("ivec3(0)", VALUE_TYPE::INT);
    else
      vector = std::dynamic_pointer_cast<Value>(nodeData);
    vector = Value::cast(vector, ::result_type(to_vector(vector->value_type), VALUE_TYPE::VEC3));
  }

  update_result();
}

std::shared_ptr<QtNodes::NodeData> RotateQuicklyNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  return rotated;
}

QWidget* RotateQuicklyNode::embeddedWidget()
{
  return _axis;
}

void RotateQuicklyNode::update_result()
{
  rotated = std::make_shared<Value>(_axis->currentData().toString().arg(vector->expression), vector->value_type);
  dataUpdated(0);
}
