#include <pointcloud_viewer/shader_nodes/switch_node.hpp>

#include <QFormLayout>
#include <QSpinBox>

SwitchNode::SwitchNode()
{
  _root_widget = new QWidget;
  QFormLayout* form = new QFormLayout;
  _root_widget->setLayout(form);

  for(int i=0; i<N; ++i)
  {
    all_classes[i] = i;

    QSpinBox* case_value = new QSpinBox;
    case_value->setValue(i);

    form->addRow(QString("case %0:").arg(i), case_value);

    connect(case_value, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [i, this](int new_value){
      all_classes[i] = new_value;
    });
  }


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
    return N+2;
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
    static_assert(index_default > index_values[N-1], "please update condition");
    if(portIndex == index_condition)
      return "Condition";
    if(portIndex == index_default)
      return "Default";
    else
      return QString("Case %0:").arg(portIndex - index_condition);
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
  if(port == index_condition)
    conditionInput = std::dynamic_pointer_cast<Value>(nodeData);
  else if(port == index_default)
    defaultValue = std::dynamic_pointer_cast<Value>(nodeData);
  else
    valuesInput[port-index_condition] = std::dynamic_pointer_cast<Value>(nodeData);
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
  output = std::make_shared<Value>("0", VALUE_TYPE::INT);

  if(defaultValue != nullptr)
    output = defaultValue;

  if(conditionInput == nullptr)
  {
    output.reset();
    dataInvalidated(0);
    return;
  }

  int num_used_cases = 0;
  for(int i=0; i<N; ++i)
    num_used_cases += valuesInput[i] != nullptr;

  if(num_used_cases == 0)
  {
    if(defaultValue != nullptr)
    {
      output = defaultValue;
      dataUpdated(0);
    }else
    {
      output.reset();
      dataInvalidated(0);
    }
    return;
  }

  if(defaultValue==nullptr && num_used_cases == 1)
  {
    for(int i=0; i<N; ++i)
      if(valuesInput[i] != nullptr)
      {
        output = valuesInput[i];
        dataUpdated(0);
        return;
      }
  }

  QString expression = "handle_switch(";

  expression += Value::cast(conditionInput, VALUE_TYPE::INT)->expression;
  expression += ", ";

  QString dummy_expression;
  int dummy_case;
  value_type_t value_type = output->value_type;

  int current_case = 0;
  for(int i=0; i<N; ++i)
  {
    if(valuesInput[i] != nullptr)
    {
      expression += QString("%0, %1, ").arg(all_classes[i]).arg(valuesInput[i]->expression);

      if(defaultValue == nullptr)
        output = valuesInput[i];
      if(current_case == 0)
      {
        if(defaultValue != nullptr)
          output = Value::cast(output, valuesInput[i]->value_type);
        dummy_case = i;
        value_type = valuesInput[i]->value_type;
        dummy_expression = Value::cast(std::make_shared<Value>("0", VALUE_TYPE::INT), valuesInput[i]->value_type)->expression;
      }

      ++current_case;
    }
  }

  for(; current_case<N; ++current_case)
    expression += QString("%0, %1, ").arg(std::numeric_limits<int>::min()).arg(dummy_expression);

  expression += output->expression;
  expression += ")";

  output = std::make_shared<Value>(expression, value_type);
  dataUpdated(0);
}
