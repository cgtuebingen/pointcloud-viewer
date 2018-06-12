#ifndef POINTCLOUDVIEWER_POINT_CLOUD_HPP_
#define POINTCLOUDVIEWER_POINT_CLOUD_HPP_

#include <geometry/aabb.hpp>

#include <QObject>
#include <vector>

struct data_type_t final
{
  enum base_type_t : uint8_t
  {
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
  };
  typedef base_type_t BASE_TYPE;

  base_type_t base_type;
  uint num_components;
  uint stride_in_bytes;
};

class Buffer final
{
public:
  Buffer();
  Buffer(Buffer&& other);
  void operator=(Buffer&& other);

  Buffer(const Buffer& buffer) = delete;
  Buffer& operator=(const Buffer& buffer) = delete;

private:
  std::vector<uchar> bytes;
};

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
  void set_data(column_t column, data_type_t input_data_type, const void* data, size_t size_in_bytes);

private:
  Buffer coordinates, colors, user_data;
};

#endif // POINTCLOUDVIEWER_POINT_CLOUD_HPP_
