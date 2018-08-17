#ifndef GEOMETRY_CONE_HPP_
#define GEOMETRY_CONE_HPP_

#include <geometry/ray.hpp>

struct cone_t final
{
public:
  glm::vec3 origin;
  float tan_half_angle;
  glm::vec3 direction;
  float inv_cos_half_angle;

  static cone_t cone_from_ray_angle(ray_t ray, float half_cone_angle);
  static cone_t cone_from_ray_tan_angle(ray_t ray, float tan_half_cone_angle);

  bool contains(glm::vec3 point) const;

  ray_t center_ray() const;
  float half_angle() const;
  float cone_radius_at(float t) const;

  ray_t closest_ray_towards(glm::vec3 point) const;

private:
};

#include <geometry/cone.inl>

#endif // GEOMETRY_CONE_HPP_
