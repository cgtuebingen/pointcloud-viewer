#include <pointcloud_viewer/shader_nodes/value.hpp>

Value::Value()
{
}

Value::Value(QString expression, value_type_t value_type)
  : expression(expression),
    value_type(value_type)
{
}

std::shared_ptr<Value> Value::cast(std::shared_ptr<Value> value, value_type_t expected_type)
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

QtNodes::NodeDataType Value::type() const
{
  return QtNodes::NodeDataType{"value", "Value"};
}


value_type_t property_to_value_type(property_type_t property_type)
{
  switch(property_type)
  {
  case PROPERTY_TYPE::FLOAT32:
    return VALUE_TYPE::FLOAT;
  case PROPERTY_TYPE::FLOAT64:
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
