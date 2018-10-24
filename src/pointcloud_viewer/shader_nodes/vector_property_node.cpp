#include <pointcloud_viewer/shader_nodes/vector_property_node.hpp>

#include <QVBoxLayout>

VectorPropertyNode::VectorPropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types, QPixmap warning_icon)
  : property_base_types(property_base_types),
    supportedPropertyNames(supportedPropertyNames),
    missingPropertyNames(missingPropertyNames)
{
  x_combobox = new QComboBox;
  y_combobox = new QComboBox;
  z_combobox = new QComboBox;

  x_warning_widget = new QLabel;
  x_warning_widget->setToolTip("The property is not existent within the pointcloud.");
  x_warning_widget->setPixmap(warning_icon);

  y_warning_widget = new QLabel;
  y_warning_widget->setToolTip("The property is not existent within the pointcloud.");
  y_warning_widget->setPixmap(warning_icon);

  z_warning_widget = new QLabel;
  z_warning_widget->setToolTip("The property is not existent within the pointcloud.");
  z_warning_widget->setPixmap(warning_icon);

  _root_widget = new QWidget;
  QVBoxLayout* vbox = new QVBoxLayout(_root_widget);
  vbox->setMargin(0);

  auto add_row = [vbox, this, &supportedPropertyNames, &missingPropertyNames](QComboBox* combobox, QLabel* warning_widget, void(VectorPropertyNode::*changed_property)(QString)){
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
    hbox->addWidget(combobox);
    hbox->addWidget(warning_widget);

    connect(combobox, &QComboBox::currentTextChanged, this, changed_property);

    for(QString name : supportedPropertyNames)
      combobox->addItem(name);
    for(QString name : missingPropertyNames)
      combobox->addItem(name);

    vbox->addLayout(hbox);
  };

  add_row(x_combobox, x_warning_widget, &VectorPropertyNode::changedXProperty);
  add_row(y_combobox, y_warning_widget, &VectorPropertyNode::changedYProperty);
  add_row(z_combobox, z_warning_widget, &VectorPropertyNode::changedZProperty);
}

QJsonObject VectorPropertyNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  jsonObject["property_x_name"] = x_combobox->currentText();
  jsonObject["property_y_name"] = y_combobox->currentText();
  jsonObject["property_z_name"] = z_combobox->currentText();

  return jsonObject;
}

void VectorPropertyNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  set_properties(jsonObject["property_x_name"].toString(),
      jsonObject["property_y_name"].toString(),
      jsonObject["property_z_name"].toString());
}

QString VectorPropertyNode::caption() const
{
  return "VectorProperty";
}

QString VectorPropertyNode::name() const
{
  return "VectorProperty";
}

uint VectorPropertyNode::nPorts(QtNodes::PortType portType) const
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

QtNodes::NodeDataType VectorPropertyNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::Out);
  Q_ASSERT(portIndex == 0);
  return Value().type();
}

void VectorPropertyNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_UNREACHABLE();
  Q_UNUSED(nodeData);
  Q_UNUSED(portIndex);
}

std::shared_ptr<QtNodes::NodeData> VectorPropertyNode::outData(QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);
  return _vector_property;
}

void VectorPropertyNode::set_properties(const QString& nameX, const QString& nameY, const QString& nameZ)
{
  auto set_property = [this](QComboBox* comboBox, QString name){
    if(supportedPropertyNames.contains(name) || missingPropertyNames.contains(name))
      comboBox->setCurrentText(name);
  };

  set_property(x_combobox, nameX);
  set_property(y_combobox, nameY);
  set_property(z_combobox, nameZ);
}

QWidget* VectorPropertyNode::embeddedWidget()
{
  return _root_widget;
}

void VectorPropertyNode::changedXProperty(QString name)
{
  x_warning_widget->setVisible(!supportedPropertyNames.contains(name));

  value_type_t type = property_to_value_type(property_base_types[name]);

  _x_property = std::make_shared<Value>(name, type);
  changedProperty();
}

void VectorPropertyNode::changedYProperty(QString name)
{
  y_warning_widget->setVisible(!supportedPropertyNames.contains(name));

  value_type_t type = property_to_value_type(property_base_types[name]);

  _y_property = std::make_shared<Value>(name, type);
  changedProperty();
}

void VectorPropertyNode::changedZProperty(QString name)
{
  z_warning_widget->setVisible(!supportedPropertyNames.contains(name));

  value_type_t type = property_to_value_type(property_base_types[name]);

  _z_property = std::make_shared<Value>(name, type);
  changedProperty();
}

void VectorPropertyNode::changedProperty()
{
  value_type_t type = to_vector(result_type(_x_property->value_type, _y_property->value_type, _z_property->value_type));

  _vector_property = std::make_shared<Value>(QString("%0(%1, %2, %3)").arg(format(type)).arg(_x_property->expression).arg(_y_property->expression).arg(_z_property->expression), type);
  dataUpdated(0);
}
