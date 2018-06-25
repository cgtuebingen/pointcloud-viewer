#include <pointcloud/pointcloud.hpp>
#include <core_library/print.hpp>
#include <cstring>

#include <core_library/types.hpp>

#include <QtGlobal>

typedef data_type_t::BASE_TYPE BASE_TYPE;

PointCloud::PointCloud()
{
}

PointCloud::PointCloud(PointCloud&& other)
  : PointCloud()
{
  *this = std::move(other);
}

PointCloud& PointCloud::operator=(PointCloud&& other)
{
  std::swap(this->aabb, other.aabb);
  std::swap(this->num_points, other.num_points);
  this->coordinate_color = std::move(other.coordinate_color);
  this->user_data = std::move(other.user_data);

  return *this;
}

void PointCloud::clear()
{
  coordinate_color.clear();
  user_data.clear();

  aabb.min_point = glm::vec3(std::numeric_limits<float>::max());
  aabb.max_point = glm::vec3(-std::numeric_limits<float>::max());
}

void PointCloud::resize(size_t num_points)
{
  this->num_points = num_points;

  coordinate_color.resize(num_points * 4*4);
  user_data.resize(num_points * 4*4);

  coordinate_color.memset(0xffffffff);
}

void PointCloud::set_data(PointCloud::column_t column, data_type_t input_data_type, const uint8_t* data, size_t first_vertex_to_set, size_t num_vertices_to_set)
{
  data_type_t internal_data_type;

  Q_ASSERT(num_points >= first_vertex_to_set + num_vertices_to_set);

  const size_t size_in_bytes = num_vertices_to_set * input_data_type.stride_in_bytes;
  const size_t offset_in_bytes = first_vertex_to_set * input_data_type.stride_in_bytes;

  switch(column)
  {
  case COLUMN::COORDINATES:
    internal_data_type.base_type = BASE_TYPE::FLOAT32;
    internal_data_type.num_components = 3;
    internal_data_type.stride_in_bytes = 4*4;
    coordinate_color.fill(internal_data_type, input_data_type, data, size_in_bytes, /* offset: */ offset_in_bytes);
    break;
  case COLUMN::COLOR:
    internal_data_type.base_type = BASE_TYPE::UINT8_NORMALIZED;
    internal_data_type.num_components = 3;
    internal_data_type.stride_in_bytes = 4*4;
    coordinate_color.fill(internal_data_type, input_data_type, data, size_in_bytes, /* offset: */ offset_in_bytes+3*4); // Use the 4 bytes directly after  of the coordinates (which would remain as useless padding) for storing the color.
    break;
  case COLUMN::USER_DATA:
    internal_data_type.base_type = BASE_TYPE::FLOAT32;
    internal_data_type.num_components = 4;
    internal_data_type.stride_in_bytes = 4*4;
    user_data.fill(internal_data_type, input_data_type, data, size_in_bytes, /* offset: */ offset_in_bytes);
    break;
  }
}
