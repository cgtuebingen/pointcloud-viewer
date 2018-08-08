#ifndef POINTCLOUDVIEWER_POINTCLOUD_HPP_
#define POINTCLOUDVIEWER_POINTCLOUD_HPP_

#include <pointcloud/buffer.hpp>
#include <geometry/aabb.hpp>

/*
Stores the whole point cloud
*/
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

  struct vertex_t
  {
    glm::vec3 coordinate;
    glm::u8vec3 color;
    padding<uint8_t> _padding;
  };

  Buffer coordinate_color, user_data;
  aabb_t aabb;
  size_t num_points;
  bool is_valid;

  PointCloud();
  PointCloud(PointCloud&& other);
  PointCloud& operator=(PointCloud&& other);

  void clear();
  void resize(size_t num_points);
  void set_data(column_t column, data_type_t input_data_type, const uint8_t* data, size_t first_vertex_to_set, size_t num_vertices_to_set);
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_HPP_
