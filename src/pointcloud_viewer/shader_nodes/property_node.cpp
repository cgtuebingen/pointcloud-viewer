#include <pointcloud_viewer/shader_nodes/property_node.hpp>

#include <QVBoxLayout>

PropertyNode::PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types, QPixmap warning_icon)
  : property_base_types(property_base_types),
    supportedPropertyNames(supportedPropertyNames),
    missingPropertyNames(missingPropertyNames)
{
  _combobox = new QComboBox;

  _warning_widget = new QLabel;
  _warning_widget->setToolTip("The property is not existent within the pointcloud.");
  _warning_widget->setPixmap(warning_icon);

  _hbox_widget = new QWidget;
  QHBoxLayout* hbox = new QHBoxLayout(_hbox_widget);
  hbox->setMargin(0);
  hbox->addWidget(_combobox);
  hbox->addWidget(_warning_widget);

  connect(_combobox, &QComboBox::currentTextChanged, this, &PropertyNode::changedProperty);

  for(QString name : supportedPropertyNames)
    _combobox->addItem(name);
  for(QString name : missingPropertyNames)
    _combobox->addItem(name);
}

QJsonObject PropertyNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["property_name"] = _combobox->currentText();

  return jsonObject;
}

void PropertyNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  set_property(jsonObject["property_name"].toString());
}

void PropertyNode::set_property(const QString& name)
{
  auto set_property = [this](QComboBox* comboBox, QString name){
    if(supportedPropertyNames.contains(name) || missingPropertyNames.contains(name))
      comboBox->setCurrentText(name);
  };

  set_property(_combobox, name);
}

QString PropertyNode::property_name() const
{
  return _combobox->currentText();
}

QString PropertyNode::caption() const
{
  return "Property";
}

QString PropertyNode::name() const
{
  return "Property";
}

uint PropertyNode::nPorts(QtNodes::PortType portType) const
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

QtNodes::NodeDataType PropertyNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void PropertyNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_UNREACHABLE();
  Q_UNUSED(nodeData);
  Q_UNUSED(portIndex);
}

std::shared_ptr<QtNodes::NodeData> PropertyNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);
  return _property;
}

QWidget* PropertyNode::embeddedWidget()
{
  return _hbox_widget;
}

void PropertyNode::changedProperty(QString name)
{
  _warning_widget->setVisible(!supportedPropertyNames.contains(name));

  value_type_t type = property_to_value_type(property_base_types[name]);

  _property = std::make_shared<Value>(name, type);
  dataUpdated(0);
}
