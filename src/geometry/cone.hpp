#ifndef GEOMETRY_CONE_HPP_
#define GEOMETRY_CONE_HPP_

#include <geometry/ray.hpp>

// Cone structure
//
// used for compensating perspective of a mouse click
//
// The cone has an angle describing the width of the cone. But it also has a
// half angle, whic his the angle from one side of the cone to the ray in the center of the cone
//
struct cone_t final
{
public:
  glm::vec3 origin;
  float tan_half_angle; // precomputed tangens of the half angle. Used to quickly get he width of the cone for a distance
  glm::vec3 direction;
  float inv_cos_half_angle; // procumputed inverse of the cos of a half angle. As this code is imported from anothe rproject of mine, it's not used here. But it would be usefull, if we would introduce a cone/sphere intersectoin test

  static cone_t cone_from_ray_angle(ray_t ray, float half_cone_angle);
  static cone_t cone_from_ray_tan_angle(ray_t ray, float tan_half_cone_angle);

  // Returns true, if a point is within the shape described by the cone
  bool contains(glm::vec3 point) const;

  // Returns the ray in the center of the cone
  ray_t center_ray() const;
  // Returns the half angle (anjgle from the center_ray() to one side of the cone)
  float half_angle() const;
  // Returns the width of a cone at the given distance t
  float cone_radius_at(float t) const;

  // Returns the closest ray within the cone (sharing the origin with the cone) to the given point, which may or may not be within the cone.
  // Used for a cone/aabb intersection test (not exact, but doesn't have to be)
  ray_t closest_ray_towards(glm::vec3 point) const;

private:
};

#include <geometry/cone.inl>

#endif // GEOMETRY_CONE_HPP_
