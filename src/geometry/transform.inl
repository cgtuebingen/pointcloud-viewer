#include <geometry/transform.hpp>
#include <geometry/swizzle.hpp>

/**
Simple point transformation.
*/
inline glm::vec3 transform_point(const glm::mat4& t, const glm::vec3& point)
{
  glm::vec4 vector(point, 1);

  vector = t * vector;

  return xyz(vector) / vector.w;
}

inline glm::vec3 transform_point(const glm::mat4x3& t, const glm::vec3& point)
{
  glm::vec4 vector(point, 1);

  return t * vector;
}

/**
Simple vector transformation.
*/
inline glm::vec3 transform_direction(const glm::mat4& t, const glm::vec3& relative)
{
  glm::vec4 vector(relative, 0);

  vector = t * vector;

  return xyz(vector);
}

inline glm::vec3 transform_direction(const glm::mat4x3& t, const glm::vec3& relative)
{
  glm::vec4 vector(relative, 0);

  return t * vector;
}
