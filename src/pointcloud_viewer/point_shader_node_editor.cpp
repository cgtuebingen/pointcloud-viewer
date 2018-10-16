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
  PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types);

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

PropertyNode::PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, property_type_t> property_base_types)
  : property_base_types(property_base_types),
    supportedPropertyNames(supportedPropertyNames),
    missingPropertyNames(missingPropertyNames)
{
  const QStyle* style = QApplication::style();

  _combobox = new QComboBox;

  _warning_widget = new QLabel;
  _warning_widget->setToolTip("The property is not existent within the pointcloud.");
  _warning_widget->setPixmap(style->standardIcon(QStyle::SP_MessageBoxWarning).pixmap(QSize(22,22)));

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

  value_type_t vector_type = to_vector(result_type(result_type(x->value_type, y->value_type), z->value_type));
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

// ==== PointShader::edit ================

void PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;


  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
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

  // TODO

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

  registry->registerModel<MakeVectorNode>("Vector");
  registry->registerModel<SplitVectorNode>("Vector");
  registry->registerModel<OutputNode>("Output");
  registry->registerModel<SpyNode>("Output");
  registry->registerModel<PropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames, base_type_for_name]() {
    return std::make_unique<PropertyNode>(supportedPropertyNames, missingPropertyNames, base_type_for_name);
  });

  QtNodes::FlowScene* flowScene = new QtNodes::FlowScene(registry);
  QtNodes::FlowView* flowView = new QtNodes::FlowView(flowScene);
  flowView->setMinimumSize(640, 480);
  vbox->addWidget(flowView);

  QDialogButtonBox* buttonGroup = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  vbox->addWidget(buttonGroup);
  QObject::connect(buttonGroup, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  QObject::connect(buttonGroup, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  dialog.setLayout(vbox);

  QObject::connect(&dialog, &QDialog::accepted, [](){
    // TODO
  });

  dialog.exec();
}
