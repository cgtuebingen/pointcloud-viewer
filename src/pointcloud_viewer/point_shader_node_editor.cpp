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

typedef data_type::base_type_t base_type_t;
typedef data_type::BASE_TYPE BASE_TYPE;

class Value final : public QtNodes::NodeData
{
public:
  QString expression;
  base_type_t base_type;
  uint num_components;
  QStringList errors;

  Value():Value("0.0", BASE_TYPE::FLOAT32, 1){}
  Value(QString expression, base_type_t base_type, uint num_components):expression(expression),base_type(base_type),num_components(num_components) {}

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
  PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, base_type_t> property_base_types);

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
  QMap<QString, base_type_t> property_base_types;
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _property = std::make_shared<Value>();

  QWidget* _hbox_widget;
  QLabel* _warning_widget;
  QComboBox* _combobox;
};

PropertyNode::PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames, QMap<QString, base_type_t> property_base_types)
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

  _property = std::make_shared<Value>(name, property_base_types[name], 1);
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
    _label_type->setText("(" + toString(value->base_type) + (value->num_components==1 ? QString() : QString("[%0]").arg(value->num_components)) + ")");
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

// ==== PointShader::edit ================

void PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;


  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  QMap<QString, base_type_t> base_type_for_name;
  if(currentPointcloud == nullptr)
  {
    missingPropertyNames << "x" << "y" << "z" << "red" << "green" << "blue";
    for(property_t property  : properties())
      missingPropertyNames << property.name;

    base_type_for_name["x"]   = base_type_for_name["y"]    = base_type_for_name["z"]     = BASE_TYPE::FLOAT32;
    base_type_for_name["red"] = base_type_for_name["green"] = base_type_for_name["blue"] = BASE_TYPE::UINT8;
  }else
  {
    supportedPropertyNames << currentPointcloud->user_data_names.toList();
    for(property_t property  : properties())
      if(!supportedPropertyNames.contains(property.name))
        missingPropertyNames << property.name;

    for(int i=0; i<currentPointcloud->user_data_names.length(); ++i)
      base_type_for_name[currentPointcloud->user_data_names[i]] = currentPointcloud->user_data_types[i];
  }

  if(supportedPropertyNames.isEmpty())
    supportedPropertyNames << "x" << "y" << "z";

  // TODO

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

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
