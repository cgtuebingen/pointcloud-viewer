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
  QString caption() const override{return "Property";}
  QString name() const override{return "property";}
  uint nPorts(QtNodes::PortType portType) const override;
  QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

  void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port) override;

  std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override;

  QWidget* embeddedWidget() override;

private:
  std::shared_ptr<ScalarExpression> _property = std::make_shared<ScalarExpression>();
};

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

  // TODO

  std::shared_ptr<QtNodes::DataModelRegistry> registry(new QtNodes::DataModelRegistry);

  registry->registerModel<PropertyNode>("Input");

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
