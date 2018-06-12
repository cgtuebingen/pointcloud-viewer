#ifndef POINTCLOUDVIEWER_POINT_CLOUD_HPP_
#define POINTCLOUDVIEWER_POINT_CLOUD_HPP_

#include <QObject>
#include <vector>
#include <glm/glm.hpp>
#include <tinyply.h>

class PointCloud : public QObject
{
Q_OBJECT

public:
  std::vector<glm::vec3> coordinates;
  std::vector<glm::u8vec3> colors;
  glm::vec3 aabb_min, aabb_max;
};

#endif // POINTCLOUDVIEWER_POINT_CLOUD_HPP_
