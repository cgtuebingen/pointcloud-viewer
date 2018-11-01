#include <pointcloud_viewer/shader_nodes/switch_node.hpp>

#include <QFormLayout>

void remove_focus_after_enter(QAbstractSpinBox* w);

SwitchNode::SwitchNode()
{
  _root_widget = new QWidget;
  QFormLayout* form = new QFormLayout;
  _root_widget->setLayout(form);

  for(int i=0; i<N; ++i)
  {
    all_classes[i] = i;

    case_widget[i] = new QSpinBox;
    case_widget[i]->setValue(i);

    form->addRow(QString("case %0:").arg(i), case_widget[i]);

    connect(case_widget[i], static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [i, this](int new_value){
      all_classes[i] = new_value;
      update_result();
    });
    remove_focus_after_enter(case_widget[i]);
  }

  update_result();
}

QJsonObject SwitchNode::save() const
{
  QJsonObject jsonObject = QtNodes::NodeDataModel::save();

  for(int i=0; i<N; ++i)
    jsonObject[QString("case%0").arg(i)] = all_classes[i];

  return jsonObject;
}

void SwitchNode::restore(const QJsonObject& jsonObject)
{
  QtNodes::NodeDataModel::restore(jsonObject);

  for(int i=0; i<N; ++i)
  {
    all_classes[i] = jsonObject[QString("case%0").arg(i)].toInt();
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
      return QString("Case %0:").arg(portIndex - index_condition - 1);
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
    valuesInput[port-index_condition - 1] = std::dynamic_pointer_cast<Value>(nodeData);
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
  if(conditionInput == nullptr)
  {
    output.reset();
    dataInvalidated(0);
    return;
  }

  bool has_default_value = defaultValue!=nullptr;
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

  QString condition = Value::cast(conditionInput, VALUE_TYPE::INT)->expression;
  QString expression = "(";

  QString dummy_expression;
  value_type_t value_type = defaultValue==nullptr ? VALUE_TYPE::INT : defaultValue->value_type;

  int current_case = 0;
  for(int i=0; i<N; ++i)
  {
    if(valuesInput[i] != nullptr)
    {
      if(current_case==0)
        value_type = valuesInput[i]->value_type;

      QString current_expression = Value::cast(valuesInput[i], value_type)->expression;

      bool is_last = num_used_cases+has_default_value == current_case+1;

      if(is_last)
      {
        expression += current_expression;
      }else
      {
        expression += QString("%0==%1 ? ").arg(condition).arg(all_classes[i]);
        expression += current_expression;
        expression += " : ";
      }

      ++current_case;
    }
  }

  if(defaultValue != nullptr)
    expression += Value::cast(defaultValue, value_type)->expression;
  expression += ")";

  output = std::make_shared<Value>(expression, value_type);
  dataUpdated(0);
}
