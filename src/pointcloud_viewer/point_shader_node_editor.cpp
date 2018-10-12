#include <pointcloud_viewer/point_shader.hpp>
#include <pointcloud/buffer.hpp>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/NodeDataModel>

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

class ScalarExpression final : public QtNodes::NodeData
{
public:
  QString expression;
  QStringList errors;

  ScalarExpression(QString expression="42"):expression(expression){}

  QtNodes::NodeDataType type() const;
};

QtNodes::NodeDataType ScalarExpression::type() const
{
  return QtNodes::NodeDataType{"scalar", "Scalar"};
}

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

private:
  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  std::shared_ptr<ScalarExpression> _property = std::make_shared<ScalarExpression>();
};

PropertyNode::PropertyNode(QStringList supportedPropertyNames, QStringList missingPropertyNames)
  : supportedPropertyNames(supportedPropertyNames),
    missingPropertyNames(missingPropertyNames)
{
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
  return ScalarExpression().type();
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
  return nullptr;
}

void PointShader::edit(QWidget* parent, const QSharedPointer<PointCloud>& currentPointcloud)
{
  QDialog dialog(parent);
  dialog.setModal(true);

  QVBoxLayout* vbox = new QVBoxLayout;


  QStringList supportedPropertyNames;
  QStringList missingPropertyNames;
  if(currentPointcloud == nullptr)
    supportedPropertyNames << "x" << "y" << "z" << "red" << "green" << "blue";
  else
    supportedPropertyNames << currentPointcloud->user_data_names.toList();
  for(Implementation::property_t property  : _implementation->usedProperties)
    if(!supportedPropertyNames.contains(property.name))
      missingPropertyNames << property.name;

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
