#include <pointcloud/pointcloud.hpp>
#include <core_library/print.hpp>
#include <cstring>

#include <core_library/types.hpp>

#include <QtGlobal>
#include <QDebug>

typedef data_type::BASE_TYPE BASE_TYPE;

PointCloud::PointCloud()
{
  is_valid = false;
}

PointCloud::PointCloud(PointCloud&& other) = default;

PointCloud& PointCloud::operator=(PointCloud&& other) = default;

PointCloud::UserData PointCloud::all_values_of_point(size_t point_index) const
{
  const int n = user_data_names.length();

  QVector<QVariant> values;
  values.reserve(n);

  const uint8_t* data = user_data.data() + user_data_stride * point_index;

  for(int i=0; i<n; ++i)
  {
    QVariant value;
    switch(user_data_types[i])
    {
    case BASE_TYPE::UINT8:
    case BASE_TYPE::UINT16:
    case BASE_TYPE::UINT32:
      value = qulonglong(data_type::read_value_from_buffer<uint64_t>(user_data_types[i], data + user_data_offset[i]));
      break;
    case BASE_TYPE::INT8:
    case BASE_TYPE::INT16:
    case BASE_TYPE::INT32:
      value = qlonglong(data_type::read_value_from_buffer<int64_t>(user_data_types[i], data + user_data_offset[i]));
      break;
    case BASE_TYPE::FLOAT32:
    case BASE_TYPE::FLOAT64:
      value = double(data_type::read_value_from_buffer<float64_t>(user_data_types[i], data + user_data_offset[i]));
      break;
    }

    values << value;
  }

  return UserData{user_data_names, values};
}

void PointCloud::clear()
{
  coordinate_color.clear();
  user_data.clear();
  kdtree_index.clear();

  aabb.min_point = glm::vec3(std::numeric_limits<float>::max());
  aabb.max_point = glm::vec3(-std::numeric_limits<float>::max());

  user_data_stride = 0;
  user_data_names.clear();
  user_data_offset.clear();
  user_data_types.clear();
}

void PointCloud::resize(size_t num_points)
{
  this->num_points = num_points;
  this->is_valid = true;

  coordinate_color.resize(num_points * stride);
  user_data.resize(num_points * user_data_stride);

  coordinate_color.memset(0xffffffff);
  user_data.memset(0xffffffff);
}

void PointCloud::set_user_data_format(size_t user_data_stride, QVector<QString> user_data_names, QVector<size_t> user_data_offset, QVector<data_type::base_type_t> user_data_types)
{
  this->user_data_stride = user_data_stride;
  this->user_data_names = user_data_names;
  this->user_data_offset = user_data_offset;
  this->user_data_types = user_data_types;
}

void PointCloud::build_kd_tree(std::function<bool(size_t, size_t)> feedback)
{
  kdtree_index.build(aabb, coordinate_color.data(), num_points, stride, feedback);
}

bool PointCloud::can_build_kdtree() const
{
  return this->num_points>0 && !kdtree_index.is_initialized();
}

bool PointCloud::has_build_kdtree() const
{
  return this->num_points>0 && kdtree_index.is_initialized();
}

QDebug operator<<(QDebug debug, const PointCloud::UserData& userData)
{
  debug.nospace() << "/==== UserData ====\\\n";

  for(int i=0; i<userData.names.length(); ++i)
    debug.nospace().noquote() << "| " << userData.names[i] << ": " << userData.values[i].toString() << "\n";
  debug.nospace() << "\\==================/\n";

  return debug;
}

// ==== PointCloud::Shader ====

QStringList PointCloud::Shader::ordered_properties() const
{
  QStringList ordered_properties = used_properties.toList();
  ordered_properties.sort();
  return ordered_properties;
}

bool PointCloud::Shader::is_empty() const
{
  return coordinate_expression.isEmpty() || color_expression.isEmpty() || node_data.isEmpty();
}

void PointCloud::Shader::export_to_file(QString filename) const
{
  // TODO
  throw "TODO: implement";
}

PointCloud::Shader PointCloud::Shader::import_from_file(QString filename)
{
  // TODO
  throw "TODO: implement";
}
