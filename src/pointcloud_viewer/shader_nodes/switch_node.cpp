#include <pointcloud_viewer/shader_nodes/switch_node.hpp>

#include <QFormLayout>

SwitchNode::SwitchNode()
{
  _root_widget = new QWidget;
  QFormLayout* form = new QFormLayout;
  _root_widget->setLayout(form);

  for(int i=0; i<N; ++i)


  update_result();
}

QString SwitchNode::caption() const
{
  return "Switch";
}

QString SwitchNode::name() const
{
  return "Switch";
}

uint SwitchNode::nPorts(QtNodes::PortType portType) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    return N+1;
  case QtNodes::PortType::Out:
    return 1;
  case QtNodes::PortType::None:
    return 0;
  }
  return 0;
}

QString SwitchNode::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
  switch(portType)
  {
  case QtNodes::PortType::In:
    if(portIndex == 0)
      return "Condition";
    else
      return QString("Case %0:").arg(portIndex - 1);
  case QtNodes::PortType::Out:
    return "Value";
  case QtNodes::PortType::None:
    break;
  }

  return QString();
}

bool SwitchNode::portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex) const
{
  return true;
}

QtNodes::NodeDataType SwitchNode::dataType(QtNodes::PortType, QtNodes::PortIndex) const
{
  return Value().type();
}

void SwitchNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, QtNodes::PortIndex port)
{
  if(port == 0)
    conditionInput = std::dynamic_pointer_cast<Value>(nodeData);
  else
    valuesInput[port-1] = std::dynamic_pointer_cast<Value>(nodeData);
  update_result();
}

std::shared_ptr<QtNodes::NodeData> SwitchNode::outData(QtNodes::PortIndex)
{
  return output;
}

QWidget* SwitchNode::embeddedWidget()
{
  return _root_widget;
}

void SwitchNode::update_result()
{
  TODO
}
