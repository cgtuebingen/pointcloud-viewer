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
  data_type::base_type_t base_type;
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
  PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames);

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
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<Value> _property = std::make_shared<Value>();

  QWidget* _hbox_widget;
  QLabel* _warning_widget;
  QComboBox* _combobox;
};

PropertyNode::PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames)
  : supportedPropertyNames(supportedPropertyNames),
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

  _property = std::make_shared<Value>(name, property_base_type[name], 1);
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
  QLabel* _label;
};

SpyNode::SpyNode()
{
  _label = new QLabel;
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

  auto value = std::dynamic_pointer_cast<Value>(nodeData);

  _label->setText(value->expression);
}

std::shared_ptr<QtNodes::NodeData> SpyNode::outData(QtNodes::PortIndex portIndex)
{
  Q_UNREACHABLE();
  Q_UNUSED(portIndex);
}

QWidget* SpyNode::embeddedWidget()
{
  return _label;
}

// ==== PointShader::edit ================

void PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;


  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  if(currentPointcloud == nullptr)
  {
    missingPropertyNames << "x" << "y" << "z" << "red" << "green" << "blue";
    for(property_t property  : properties())
      missingPropertyNames << property.name;
  }else
  {
    supportedPropertyNames << currentPointcloud->user_data_names.toList();
    for(property_t property  : properties())
      if(!supportedPropertyNames.contains(property.name))
        missingPropertyNames << property.name;
  }

  if(supportedPropertyNames.isEmpty())
    supportedPropertyNames << "x" << "y" << "z";

  // TODO

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

  registry->registerModel<PropertyNode>("Input",
                                        [supportedPropertyNames, missingPropertyNames]() {
    return std::make_unique<PropertyNode>(supportedPropertyNames, missingPropertyNames);
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
