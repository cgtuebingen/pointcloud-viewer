#include <core_library/print.hpp>

#include <pointcloud_viewer/point_shader.hpp>
#include <pointcloud/buffer.hpp>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/NodeDataModel>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QStyle>
#include <QApplication>
#include <QLineEdit>

// ==== AnyExpression ================

typedef data_type::base_type_t property_type_t;
typedef data_type::BASE_TYPE PROPERTY_TYPE;

enum class value_type_t
{
  INT,
  UINT,
  FLOAT,
  DOUBLE,
  IVEC3,
  UVEC3,
  VEC3,
  DVEC3,
};
typedef value_type_t VALUE_TYPE;
Q_DECLARE_METATYPE(value_type_t);

const value_type_t all_value_types[] = {VALUE_TYPE::INT, VALUE_TYPE::UINT, VALUE_TYPE::FLOAT, VALUE_TYPE::DOUBLE, VALUE_TYPE::IVEC3, VALUE_TYPE::UVEC3, VALUE_TYPE::VEC3, VALUE_TYPE::DVEC3};

value_type_t property_to_value_type(property_type_t property_type)
{
  switch(property_type)
  {
  case PROPERTY_TYPE::FLOAT32:
    return VALUE_TYPE::FLOAT;
  case PROPERTY_TYPE::FLOAT64:
  case PROPERTY_TYPE::INT64:
  case PROPERTY_TYPE::UINT64:
    return VALUE_TYPE::DOUBLE;
  case PROPERTY_TYPE::INT8:
  case PROPERTY_TYPE::INT16:
  case PROPERTY_TYPE::INT32:
    return VALUE_TYPE::INT;
  case PROPERTY_TYPE::UINT8:
  case PROPERTY_TYPE::UINT16:
  case PROPERTY_TYPE::UINT32:
    return VALUE_TYPE::UINT;
  }

  Q_UNREACHABLE();
  return VALUE_TYPE::FLOAT;
}

const char* format(value_type_t value_type)
{
  switch(value_type)
  {
  case VALUE_TYPE::UINT:
    return "uint";
  case VALUE_TYPE::INT:
    return "int";
  case VALUE_TYPE::DOUBLE:
    return "double";
  case VALUE_TYPE::FLOAT:
    return "float";
  case VALUE_TYPE::UVEC3:
    return "uvec3";
  case VALUE_TYPE::IVEC3:
    return "ivec3";
  case VALUE_TYPE::DVEC3:
    return "dvec3";
  case VALUE_TYPE::VEC3:
    return "vec3";
  }

  Q_UNREACHABLE();
  return "<unknown type>";
}

value_type_t value_type_from_string(const QString& string, value_type_t fallback)
{
  for(value_type_t value_type : all_value_types)
    if(string == format(value_type))
      return value_type;
  return fallback;
}

bool is_vector(value_type_t value_type)
{
  switch(value_type)
  {
  case VALUE_TYPE::UINT:
  case VALUE_TYPE::INT:
  case VALUE_TYPE::DOUBLE:
  case VALUE_TYPE::FLOAT:
    return false;
  case VALUE_TYPE::UVEC3:
  case VALUE_TYPE::IVEC3:
  case VALUE_TYPE::DVEC3:
  case VALUE_TYPE::VEC3:
    return true;
  }

  Q_UNREACHABLE();
  return false;
}

value_type_t to_vector(value_type_t value_type)
{
  switch(value_type)
  {
  case VALUE_TYPE::UINT:
  case VALUE_TYPE::UVEC3:
    return VALUE_TYPE::UVEC3;
  case VALUE_TYPE::INT:
  case VALUE_TYPE::IVEC3:
    return VALUE_TYPE::IVEC3;
  case VALUE_TYPE::DOUBLE:
  case VALUE_TYPE::DVEC3:
    return VALUE_TYPE::DVEC3;
  case VALUE_TYPE::FLOAT:
  case VALUE_TYPE::VEC3:
    return VALUE_TYPE::VEC3;
  }

  Q_UNREACHABLE();
  return VALUE_TYPE::VEC3;
}

value_type_t to_scalar(value_type_t value_type)
{
  switch(value_type)
  {
  case VALUE_TYPE::UINT:
  case VALUE_TYPE::UVEC3:
    return VALUE_TYPE::UINT;
  case VALUE_TYPE::INT:
  case VALUE_TYPE::IVEC3:
    return VALUE_TYPE::INT;
  case VALUE_TYPE::DOUBLE:
  case VALUE_TYPE::DVEC3:
    return VALUE_TYPE::DOUBLE;
  case VALUE_TYPE::FLOAT:
  case VALUE_TYPE::VEC3:
    return VALUE_TYPE::FLOAT;
  }

  Q_UNREACHABLE();
  return VALUE_TYPE::VEC3;
}

value_type_t result_type(value_type_t a, value_type_t b)
{
  value_type_t result = value_type_t(glm::max<int>(int(to_scalar(a)), int(to_scalar(b))));

  if(is_vector(a) || is_vector(b))
    result = to_vector(result);

  return result;
}

value_type_t result_type(value_type_t a, value_type_t b, value_type_t c)
{
  return result_type(result_type(a, b), c);
}

class Value final : public QtNodes::NodeData
{
public:
  QString expression;
  value_type_t value_type;
  QStringList errors;

  Value()
    : Value("0.0", VALUE_TYPE::FLOAT)
  {
  }
  Value(QString expression, value_type_t value_type)
    : expression(expression),
      value_type(value_type)
  {
  }

  static std::shared_ptr<Value> cast(std::shared_ptr<Value> value, value_type_t expected_type)
  {
    if(value->value_type == expected_type)
      return value;

    if(is_vector(value->value_type) && !is_vector(expected_type))
      value = std::make_shared<Value>("to_scalar(" + value->expression + ")", to_scalar(value->value_type));

    if(!is_vector(value->value_type) && is_vector(expected_type))
      value = std::make_shared<Value>(QString(format(to_vector(value->value_type))) + "(" + value->expression + ")", to_vector(value->value_type));

    if(value->value_type != expected_type)
      value = std::make_shared<Value>(QString(format(expected_type)) + "(" + value->expression + ")", to_scalar(value->value_type));

    return value;
  }

  QtNodes::NodeDataType type() const;
};

QtNodes::NodeDataType Value::type() const
{
  return QtNodes::NodeDataType{"value", "Value"};
}

// ==== PropertyNode ================

class PropertyNode final : public QtNodes::NodeDataModel
{
public:
  PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_type, QPixmap warning_icons);

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_property(const QString& name);

  QString caption() const override{return "Property";}
  QString name() const override{return "Property";}
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private slots:
  void changedProperty(QString name);

private:
  QMap<QString, property_type_t> property_base_types;
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _property = std::make_shared<Value>();

  QWidget* _hbox_widget;
  QLabel* _warning_widget;
  QComboBox* _combobox;
};

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

QtNodes::NodeDataType PropertyNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::Out);
  Q_ASSERT(portIndex == 0);
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

// ==== VectorPropertyNode ================

class VectorPropertyNode final : public QtNodes::NodeDataModel
{
public:
  VectorPropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types, QPixmap warning_icon);

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  QString caption() const override{return "VectorProperty";}
  QString name() const override{return "VectorProperty";}
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  void set_properties(const QString& nameX, const QString& nameY, const QString& nameZ);

  QWidget* embeddedWidget() override;

private slots:
  void changedXProperty(QString name);
  void changedYProperty(QString name);
  void changedZProperty(QString name);
  void changedProperty();

private:
  QMap<QString, property_type_t> property_base_types;
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _x_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _y_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _z_property = std::make_shared<Value>("0", VALUE_TYPE::INT);
  std::shared_ptr<Value> _vector_property = std::make_shared<Value>("ivec3(0)", VALUE_TYPE::IVEC3);

  QWidget* _root_widget;
  QLabel* x_warning_widget;
  QLabel* y_warning_widget;
  QLabel* z_warning_widget;

  QComboBox* x_combobox;
  QComboBox* y_combobox;
  QComboBox* z_combobox;
};

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

// ==== SpyNode ================

class SpyNode final : public QtNodes::NodeDataModel
{
public:
  SpyNode();

  QString caption() const override{return "Spy";}
  QString name() const override{return "Spy";}
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  QLabel* _label_expression;
  QLabel* _label_type;
  QWidget* _root;
};

SpyNode::SpyNode()
{
  _label_expression = new QLabel;
  _label_type = new QLabel;

  QVBoxLayout* vbox = new QVBoxLayout;
  _root = new QWidget();
  _root->setLayout(vbox);

  vbox->addWidget(_label_expression);
  vbox->addWidget(_label_type);
  vbox->setMargin(0);

  QFont font = _label_expression->font();
  font.setStyleHint(QFont::Monospace);
  font.setPointSizeF(font.pointSizeF()*1.5);
  _label_expression->setFont(font);
  _label_expression->setAlignment(Qt::AlignHCenter);
  _label_type->setAlignment(Qt::AlignRight);
}

uint SpyNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return 1;
  case QtNodes::PortType::Out:
    return 0;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QtNodes::NodeDataType SpyNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  Q_ASSERT(portType == QtNodes::PortType::In);
  Q_ASSERT(portIndex == 0);
  return Value().type();
}

void SpyNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex portIndex)
{
  Q_ASSERT(portIndex == 0);

  if(nodeData == nullptr)
  {
    _label_expression->setText(QString());
    _label_type->setText(QString());
  }else
  {
    auto value = std::dynamic_pointer_cast<Value>(nodeData);

    _label_expression->setText(value->expression.toHtmlEscaped());
    _label_expression->setMinimumWidth(_label_expression->fontMetrics().width(_label_expression->text())+2);
    _label_type->setText("(" + QString(format(value->value_type)) + ")");
    _label_type->setMinimumWidth(_label_type->fontMetrics().width(_label_type->text())+2);
  }
}

std::shared_ptr<QtNodes::NodeData> SpyNode::outData(QtNodes::PortIndex portIndex)
{
  Q_UNREACHABLE();
  Q_UNUSED(portIndex);
}

QWidget* SpyNode::embeddedWidget()
{
  return _root;
}

// ==== Output Node ================

class OutputNode final : public QtNodes::NodeDataModel
{
public:
  OutputNode();

  QString caption() const override{return "Output";}
  QString name() const override{return "Output";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> coordinate;
  std::shared_ptr<Value> color;

  void reset_coordinate();
  void reset_color();
};

OutputNode::OutputNode()
{
  reset_coordinate();
  reset_color();
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
  coordinate = std::make_shared<Value>("vec3(0)", VALUE_TYPE::VEC3);
}

void OutputNode::reset_color()
{
  coordinate = std::make_shared<Value>("uvec3(255)", VALUE_TYPE::UVEC3);
}

// ==== Make Vector ================

class MakeVectorNode final : public QtNodes::NodeDataModel
{
public:
  MakeVectorNode();

  QString caption() const override{return "MakeVector";}
  QString name() const override{return "MakeVector";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> x, y, z;
  std::shared_ptr<Value> vector;
};

MakeVectorNode::MakeVectorNode()
{
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  z = std::make_shared<Value>("0", VALUE_TYPE::INT);
  vector = std::make_shared<Value>("ivec3(0,0,0)", VALUE_TYPE::IVEC3);
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

// ==== Split Vector ================

class SplitVectorNode final : public QtNodes::NodeDataModel
{
public:
  SplitVectorNode();

  QString caption() const override{return "SplitVector";}
  QString name() const override{return "SplitVector";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> vector;
  std::shared_ptr<Value> x, y, z;
};

SplitVectorNode::SplitVectorNode()
{
  vector = std::make_shared<Value>("ivec3(0,0,0)", VALUE_TYPE::IVEC3);
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  z = std::make_shared<Value>("0", VALUE_TYPE::INT);
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
    x = std::make_shared<Value>(QString("(%0).x").arg(vector->expression), scalar_type);
    y = std::make_shared<Value>(QString("(%0).y").arg(vector->expression), scalar_type);
    z = std::make_shared<Value>(QString("(%0).z").arg(vector->expression), scalar_type);
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

// ==== Math Operator ================

class MathOperatorNode final : public QtNodes::NodeDataModel
{
public:
  MathOperatorNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_operator(QString op);

  QString caption() const override{return "Operator";}
  QString name() const override{return "Operator";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> x, y;
  std::shared_ptr<Value> result;
  QString operator_symbol;
  QComboBox* _combobox_op;

  void update_result();
  void update_operator();
};

MathOperatorNode::MathOperatorNode()
{
  x = std::make_shared<Value>("0", VALUE_TYPE::INT);
  y = std::make_shared<Value>("0", VALUE_TYPE::INT);
  result = std::make_shared<Value>("0", VALUE_TYPE::INT);

  _combobox_op = new QComboBox;
  _combobox_op->addItem("add (x + y)", "+");
  _combobox_op->addItem("subtract (x - z)", "-");
  _combobox_op->addItem("multiplicate (x * z)", "*");
  _combobox_op->addItem("divide (x / z)", "/");
  _combobox_op->addItem("bitwise and (x & z)", "&");
  _combobox_op->addItem("bitwise or (x | z)", "|");
  _combobox_op->addItem("bitwise xor (x ^ z)", "^");

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
    x = Value::cast(x, x->value_type);
  }else
  {
    if(nodeData == nullptr)
      y = std::make_shared<Value>("0", VALUE_TYPE::INT);
    else
      y = std::dynamic_pointer_cast<Value>(nodeData);
    y = Value::cast(y, y->value_type);
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
  value_type_t result_type = to_vector(::result_type(x->value_type, y->value_type));
  result = std::make_shared<Value>(QString("%0 %2 %1").arg(x->expression).arg(y->expression).arg(operator_symbol), result_type);
  dataUpdated(0);
}

void MathOperatorNode::update_operator()
{
  operator_symbol = _combobox_op->currentData().toString();
  update_result();
}

// ==== ValueNode ================

class ValueNode final : public QtNodes::NodeDataModel
{
public:
  ValueNode();

  QJsonObject save() const override;
  void restore(QJsonObject const & p) override;

  void set_value(QString value, value_type_t value_type);

  QString caption() const override{return "Value";}
  QString name() const override{return "Value";}
  uint nPorts(QtNodes::PortType portType) const override;
  QString portCaption(QtNodes::PortType, QtNodes::PortIndex) const override;
  bool portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<Value> value;
  QString operator_symbol;
  QLineEdit* _content_box;
  QComboBox* _combobox_type;
  QWidget* _root;

  void update_value_expression();
  void update_value_type();
};

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

// ==== PointShader::edit ================

void PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;

  std::shared_ptr<QtNodes::DataModelRegistry> registry = qt_nodes_model_registry(currentPointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  flowScene->loadFromMemory(_implementation->nodes);

  QtNodes::FlowView* flowView = new QtNodes::FlowView(flowScene);
  flowView->setMinimumSize(1024, 768);
  vbox->addWidget(flowView);

  QDialogButtonBox* buttonGroup = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  vbox->addWidget(buttonGroup);
  QObject::connect(buttonGroup, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttonGroup, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  dialog.setLayout(vbox);

  QObject::connect(&dialog, &QDialog::accepted, [flowScene, this](){
    _implementation->nodes = flowScene->saveToMemory();
  });

  dialog.exec();
}


PointShader PointShader::autogenerate(const QSharedPointer<PointCloud>& pointcloud)
{
  QSharedPointer<Implementation> implementation(new Implementation);

  implementation->name = "auto";

  if(pointcloud != nullptr)
  {
    for(int i=0; i<pointcloud->user_data_names.length(); ++i)
    {
      property_t p;

      p.name = pointcloud->user_data_names[i];
      p.type = pointcloud->user_data_types[i];
      implementation->properties << p;
    }
  }

  PointShader point_shader(implementation);

  std::shared_ptr<QtNodes::DataModelRegistry> registry = point_shader.qt_nodes_model_registry(pointcloud);

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);

  QSizeF areaSize(1024, 768);
  qreal margin = 16;

  auto set_node_position = [flowScene, areaSize, margin](QtNodes::Node& node, QPointF relative) {
    QSizeF node_size = flowScene->getNodeSize(node);
    QPointF pos(relative.x() * (areaSize.width() - margin * 2. - node_size.width()) + margin,
                relative.y() * (areaSize.height() - margin * 2. - node_size.height()) + margin);
    flowScene->setNodePosition(node, pos);
  };

  QtNodes::Node& outputNode = flowScene->createNode(std::make_unique<OutputNode>());
  set_node_position(outputNode, QPointF(1., 0.5));

  if(point_shader.contains_property("x") && point_shader.contains_property("y") && point_shader.contains_property("z"))
  {
    std::unique_ptr<QtNodes::NodeDataModel> model = registry->create("VectorProperty");

    VectorPropertyNode* node = dynamic_cast<VectorPropertyNode*>(model.get());
    Q_ASSERT(node != nullptr);

    if(node != nullptr)
    {
      node->set_properties("x", "y", "z");

      QtNodes::Node& coordinates_node = flowScene->createNode(std::move(model));
      set_node_position(coordinates_node, QPointF(0., 0.25));
      flowScene->createConnection(outputNode, 0, coordinates_node, 0);
    }
  }

  if(point_shader.contains_property("red") && point_shader.contains_property("green") && point_shader.contains_property("blue"))
  {
    std::unique_ptr<QtNodes::NodeDataModel> model = registry->create("VectorProperty");

    VectorPropertyNode* node = dynamic_cast<VectorPropertyNode*>(model.get());
    Q_ASSERT(node != nullptr);

    if(node != nullptr)
    {
      node->set_properties("red", "green", "blue");

      QtNodes::Node& coordinates_node = flowScene->createNode(std::move(model));
      set_node_position(coordinates_node, QPointF(0., 0.75));
      flowScene->createConnection(outputNode, 0, coordinates_node, 0);
    }
  }

  point_shader._implementation->nodes = flowScene->saveToMemory();

  return point_shader;
}

std::shared_ptr<QtNodes::DataModelRegistry> PointShader::qt_nodes_model_registry(const QSharedPointer<PointCloud>& currentPointcloud)
{
  QStyle* style = QApplication::style();

  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  QPixmap warning_icon = style->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(QSize(22,22));
  QMap<QString, property_type_t> base_type_for_name;
  if(currentPointcloud == nullptr)
  {
    missingPropertyNames << "x" << "y" << "z" << "red" << "green" << "blue";
    base_type_for_name["x"]   = base_type_for_name["y"]    = base_type_for_name["z"]     = PROPERTY_TYPE::FLOAT32;
    base_type_for_name["red"] = base_type_for_name["green"] = base_type_for_name["blue"] = PROPERTY_TYPE::UINT8;

    for(property_t property  : properties())
    {
      missingPropertyNames << property.name;
      base_type_for_name[property.name] = property.type;
    }
  }else
  {
    supportedPropertyNames << currentPointcloud->user_data_names.toList();
    for(property_t property  : properties())
      if(!supportedPropertyNames.contains(property.name))
      {
        missingPropertyNames << property.name;
        base_type_for_name[property.name] = property.type;
      }

    for(int i=0; i<currentPointcloud->user_data_names.length(); ++i)
      base_type_for_name[currentPointcloud->user_data_names[i]] = currentPointcloud->user_data_types[i];
  }

  if(supportedPropertyNames.isEmpty() && missingPropertyNames.isEmpty())
  {
    missingPropertyNames << "x" << "y" << "z";
    base_type_for_name["x"]   = base_type_for_name["y"]    = base_type_for_name["z"]     = PROPERTY_TYPE::FLOAT32;
  }

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

  registry->registerModel<MathOperatorNode>("Math");
  registry->registerModel<ValueNode>("Math");
  registry->registerModel<MakeVectorNode>("Vector");
  registry->registerModel<SplitVectorNode>("Vector");
  registry->registerModel<OutputNode>("Output");
  registry->registerModel<SpyNode>("Output");
  registry->registerModel<PropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon]() {
    return std::make_unique<PropertyNode>(supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon);
  });
  registry->registerModel<VectorPropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon]() {
    return std::make_unique<VectorPropertyNode>(supportedPropertyNames, missingPropertyNames, base_type_for_name, warning_icon);
  });

  return registry;
}
