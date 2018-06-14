#ifndef POINTCLOUDVIEWER_POINT_CLOUD_HPP_
#define POINTCLOUDVIEWER_POINT_CLOUD_HPP_

#include <pointcloud_viewer/buffer.hpp>
#include <geometry/aabb.hpp>

class PointCloud final
{
public:
  enum class column_t
  {
    COORDINATES,
    COLOR,
    USER_DATA,
  };
  typedef column_t COLUMN;

  aabb_t aabb;
  size_t num_points;

  PointCloud();
  PointCloud(PointCloud&& other);
  void operator=(PointCloud&& other);

  void clear();
  void set_data(column_t column, data_type_t input_data_type, const uint8_t* data, size_t size_in_bytes);

private:
  Buffer coordinate_color, user_data;
};

#endif // POINTCLOUDVIEWER_POINT_CLOUD_HPP_
