#include <geometry/cone.hpp>

inline cone_t cone_from_ray_angle(ray_t ray, float half_cone_angle)
{
  cone_t cone;
  cone.origin = ray.origin;
  cone.direction = ray.direction;
  cone.inv_cos_half_angle = 1.f / glm::cos(half_cone_angle);
  cone.tan_half_angle = glm::tan(half_cone_angle);
  return cone;
}

inline cone_t cone_from_ray_tan_angle(ray_t ray, float tan_half_cone_angle)
{
  cone_t cone;
  cone.origin = ray.origin;
  cone.direction = ray.direction;
  cone.inv_cos_half_angle = 1.f / glm::cos(glm::atan(tan_half_cone_angle));
  cone.tan_half_angle = tan_half_cone_angle;
  return cone;
}

inline ray_t cone_t::center_ray() const
{
  ray_t ray;

  ray.origin = origin;
  ray.direction = direction;

  return ray;
}

inline float cone_t::half_angle() const
{
  return glm::atan(tan_half_angle);
}

inline ray_t cone_t::closest_ray_towards(glm::vec3 point) const
{
  glm::vec3 direction_of_exact_ray = point - origin;

  {
    const float length = glm::length(direction_of_exact_ray);
    if(length < 1.e-7f)
      return center_ray();
    direction_of_exact_ray /= length;
  }

  glm::vec3 rotation_axis = glm::cross(direction, direction_of_exact_ray);
  float angle_to_exact_ray;
  {
    const float length = glm::length(rotation_axis);
    rotation_axis /= length;

    if(glm::dot(direction, direction_of_exact_ray) >= 0.f)
    {
      if(length < 1.e-7f)
        return center_ray();
      angle_to_exact_ray = glm::asin(length);
    }else
    {
      angle_to_exact_ray = glm::two_pi<float>() - glm::asin(length);
    }
  }

  const float angle_to_best_fit = glm::min(angle_to_exact_ray, half_angle());

  glm::quat rotation = glm::angleAxis(angle_to_best_fit, rotation_axis);

  ray_t best_ray = ray_t{origin, rotation * direction};

  return best_ray;
}
