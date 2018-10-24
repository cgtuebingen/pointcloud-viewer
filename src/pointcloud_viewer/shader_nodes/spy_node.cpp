#include <pointcloud_viewer/shader_nodes/spy_node.hpp>

#include <QVBoxLayout>

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
