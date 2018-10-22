#ifndef POINTCLOUDVIEWER_POINTCLOUD_HPP_
#define POINTCLOUDVIEWER_POINTCLOUD_HPP_

#include <pointcloud/buffer.hpp>
#include <pointcloud/kdtree_index.hpp>
#include <geometry/aabb.hpp>

#include <QVector>
#include <QString>
#include <QVariant>

/*
Stores the whole point cloud consisting out of the
- coordinate_color -- coordinates and colors
- user_data -- all property data
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

  struct UserData
  {
    QVector<QString> names;
    QVector<QVariant> values;
  };

  struct Shader
  {
    QString name;
    QString coordinate_expression;
    QString color_expression;
    QString node_data;
  };

  Buffer coordinate_color, user_data;
  KDTreeIndex kdtree_index;
  Shader shader;
  aabb_t aabb;
  size_t num_points;
  bool is_valid;

  size_t user_data_stride;
  QVector<QString> user_data_names;
  QVector<size_t> user_data_offset;
  QVector<data_type::base_type_t> user_data_types;

  PointCloud();
  PointCloud(PointCloud&& other);
  PointCloud& operator=(PointCloud&& other);

  constexpr static const size_t stride = 4*4;

  UserData all_values_of_point(size_t point_index) const;

  void clear();
  void resize(size_t num_points);

  void set_user_data_format(size_t user_data_stride, QVector<QString> user_data_names, QVector<size_t> user_data_offset, QVector<data_type::base_type_t> user_data_types);

  void build_kd_tree(std::function<bool(size_t, size_t)> feedback);
  bool can_build_kdtree() const;
  bool has_build_kdtree() const;
};

QDebug operator<<(QDebug debug, const PointCloud::UserData& userData);

#endif // POINTCLOUDVIEWER_POINTCLOUD_HPP_
