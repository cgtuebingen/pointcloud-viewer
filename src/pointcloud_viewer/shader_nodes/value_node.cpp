#include <pointcloud_viewer/shader_nodes/value_node.hpp>

#include <QBoxLayout>

ValueNode::ValueNode()
{
  _combobox_type = new QComboBox;
  for(value_type_t value_type : all_value_types)
    _combobox_type->addItem(format(value_type), QVariant::fromValue(value_type));

  _content_box = new QLineEdit();

  set_value("vec3(0, 0, 0)", VALUE_TYPE::VEC3);

  connect(_content_box, static_cast<void(QLineEdit::*)(const QString &)>(&QLineEdit::textChanged), this, &ValueNode::update_value_expression);
  connect(_combobox_type, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ValueNode::update_value_type);

  _root = new QWidget;
  QBoxLayout* box = new QHBoxLayout(_root);
  box->setMargin(0);
  box->addWidget(_content_box);
  box->addWidget(_combobox_type);
}

QJsonObject ValueNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["value_expression"] = value->expression;
  jsonObject["value_type"] = QString(format(value->value_type));

  return jsonObject;
}

void ValueNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  set_value(jsonObject["value_expression"].toString(),
      value_type_from_string(jsonObject["value_type"].toString(), /*fallback*/VALUE_TYPE::DVEC3));
}

void ValueNode::set_value(QString value, value_type_t value_type)
{
  this->value = std::make_shared<Value>(value, value_type);

  int index = _combobox_type->findData(QVariant::fromValue(value_type));
  if(index >= 0)
    _combobox_type->setCurrentIndex(index);
  if(value != _content_box->text())
    _content_box->setText(value);
  dataUpdated(0);
}

uint ValueNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 0;
  case QtNodes::PortType::Out:
    return 1;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString ValueNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::Out);
  Q_ASSERT(portIndex == 0);

  return "Value";
}

bool ValueNode::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::Out);
  Q_ASSERT(portIndex == 0);

  return false;
}

QtNodes::NodeDataType ValueNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void ValueNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_UNUSED(nodeData);
  Q_UNUSED(portIndex);
}

std::shared_ptr<QtNodes::NodeData> ValueNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  return value;
}

QWidget* ValueNode::embeddedWidget()
{
  return _root;
}

void ValueNode::update_value_expression()
{
  set_value(_content_box->text(), value->value_type);
}

void ValueNode::update_value_type()
{
  set_value(value->expression, _combobox_type->currentData().value<value_type_t>());
}
