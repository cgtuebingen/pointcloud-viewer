#ifndef GEOMETRY_PLANE_HPP_
#define GEOMETRY_PLANE_HPP_

#include <geometry/aabb.hpp>
#include <geometry/ray.hpp>

// Representation of a single plane
//
// Used for intersection tests
struct plane_t final
{
public:
  glm::vec3 normal;
  float d;

  static plane_t from_normal(glm::vec3 normal, glm::vec3 point);
  static plane_t from_normal(glm::vec3 normal, float d);
  static plane_t from_tangents(glm::vec3 tangent1, glm::vec3 tangent2, glm::vec3 tangent3);
  static plane_t from_three_points(glm::vec3 a, glm::vec3 b, glm::vec3 c);

  float signed_distance_to(glm::vec3 point) const;
  float distance_to(glm::vec3 point) const;
  bool is_on_frontside(glm::vec3 point) const;

  // returns infinity, if there's no intersection at all
  float intersection_distance(ray_t ray) const;
};

#include <geometry/plane.inl>

#endif // GEOMETRY_PLANE_HPP_
