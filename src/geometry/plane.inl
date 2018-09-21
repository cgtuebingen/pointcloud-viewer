#include <geometry/plane.hpp>

inline plane_t plane_t::from_normal(glm::vec3 normal, glm::vec3 point)
{
  return from_normal(normal, dot(normal, point));
}

inline plane_t plane_t::from_normal(glm::vec3 normal, float d)
{
  plane_t p;

  p.normal = normal;
  p.d = d;

  return p;
}

inline plane_t plane_t::from_tangents(glm::vec3 tangent1, glm::vec3 tangent2, glm::vec3 origin)
{
  return from_normal(normalize(cross(tangent1, tangent2)),
                     origin);
}

inline plane_t plane_t::from_three_points(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
  return from_tangents(a-c, b-c, c);
}

inline float plane_t::signed_distance_to(glm::vec3 point) const
{
  const plane_t& plane = *this;

  return glm::dot(plane.normal, point) - plane.d;
}

inline float plane_t::distance_to(glm::vec3 point) const
{
  return glm::abs(signed_distance_to(point));
}

inline bool plane_t::is_on_frontside(glm::vec3 point) const
{
  return signed_distance_to(point) > 0;
}

inline float plane_t::intersection_distance(ray_t ray) const
{
  const plane_t& plane = *this;

  // https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
  return (plane.d - dot(ray.origin, plane.normal)) / dot(ray.direction, plane.normal);
}
