#include <pointcloud_viewer/point_cloud.hpp>
#include <core_library/print.hpp>
#include <cstring>

#include <core_library/types.hpp>

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
}

void PointCloud::set_data(PointCloud::column_t column, data_type_t input_data_type, const uint8_t* data, size_t size_in_bytes)
{
  data_type_t internal_data_type;

  this->num_points = size_in_bytes / input_data_type.stride_in_bytes;

  switch(column)
  {
  case COLUMN::COORDINATES:
    internal_data_type.base_type = BASE_TYPE::FLOAT32;
    internal_data_type.num_components = 3;
    internal_data_type.stride_in_bytes = 4*4;
    coordinate_color.fill(internal_data_type, input_data_type, data, size_in_bytes);
    break;
  case COLUMN::COLOR:
    internal_data_type.base_type = BASE_TYPE::UINT8_NORMALIZED;
    internal_data_type.num_components = 3;
    internal_data_type.stride_in_bytes = 4*4;
    coordinate_color.fill(internal_data_type, input_data_type, data, size_in_bytes, /* offset: */ 3*4); // Use the 4 bytes directly after  of the coordinates (which would remain as useless padding) for storing the color.
    break;
  case COLUMN::USER_DATA:
    internal_data_type.base_type = BASE_TYPE::FLOAT32;
    internal_data_type.num_components = 4;
    internal_data_type.stride_in_bytes = 4*4;
    coordinate_color.fill(internal_data_type, input_data_type, data, size_in_bytes);
    break;
  }
}
