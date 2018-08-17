#ifndef GEOMETRY_RAY_HPP_
#define GEOMETRY_RAY_HPP_

#include <geometry/aabb.hpp>

// Representation of a single ray
//
// Used for intersection tests
struct ray_t final
{
public:
  glm::vec3 origin;
  glm::vec3 direction;

  static ray_t from_two_points(glm::vec3 from, glm::vec3 to);

  glm::vec3 get_point(float t) const;

  glm::vec3 operator[](float t) const;

  glm::vec3 nearest_point(glm::vec3 point, float* t_nearest) const;
  glm::vec3 nearest_point(glm::vec3 point) const;
  float distance_to(glm::vec3 point, float* t_nearest) const;
  float distance_to(glm::vec3 point) const;

  bool intersects_aabb(aabb_t aabb, float* intersection_distance_front, float* intersection_distance_back) const;

private:
  glm::vec3 __intersects_aabb_intersection_with_common_point(aabb_t aabb, glm::vec3 common_point, glm::bvec3* intersects) const;
  glm::vec3 __intersection_distances_to_axis_planes(glm::vec3 common_point) const;
};

#include <geometry/ray.inl>

#endif // GEOMETRY_RAY_HPP_
