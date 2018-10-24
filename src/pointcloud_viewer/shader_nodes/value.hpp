#ifndef POINTCLOUDVIEWER_SHADER_NODES_VALUE_TYPE_HPP_
#define POINTCLOUDVIEWER_SHADER_NODES_VALUE_TYPE_HPP_

#include <core_library/print.hpp>
#include <pointcloud/buffer.hpp>

#include <nodes/NodeData>

#include <QMetaType>

#include <glm/glm.hpp>

#include <memory>

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
Q_DECLARE_METATYPE(value_type_t);

const value_type_t all_value_types[] = {VALUE_TYPE::INT, VALUE_TYPE::UINT, VALUE_TYPE::FLOAT, VALUE_TYPE::DOUBLE, VALUE_TYPE::IVEC3, VALUE_TYPE::UVEC3, VALUE_TYPE::VEC3, VALUE_TYPE::DVEC3};

class Value final : public QtNodes::NodeData
{
public:
  QString expression;
  value_type_t value_type;

  Value();
  Value(QString expression, value_type_t value_type);

  static std::shared_ptr<Value> cast(std::shared_ptr<Value> value, value_type_t expected_type);

  QtNodes::NodeDataType type() const;
};

value_type_t property_to_value_type(property_type_t property_type);

const char* format(value_type_t value_type);
value_type_t value_type_from_string(const QString& string, value_type_t fallback);

bool is_vector(value_type_t value_type);

value_type_t to_vector(value_type_t value_type);
value_type_t to_scalar(value_type_t value_type);

value_type_t result_type(value_type_t a, value_type_t b);
value_type_t result_type(value_type_t a, value_type_t b, value_type_t c);

#endif // POINTCLOUDVIEWER_SHADER_NODES_VALUE_TYPE_HPP_
