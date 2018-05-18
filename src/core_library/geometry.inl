#include <core_library/geometry.hpp>

/**
The cross product betwen two vectors always returns a third vector orthogonal to
the plane, where both given vectors lie in.

The find_best_perpendicular finds a vector perpendicular to the give one. It
checks multiple candidate for the most perpendicular one and then returns the
cross product to that one.

To guarantee a valid result, provide at least two perpendicular vectors, as done
by find_best_perpendicular(vec3)
*/
inline glm::vec3 find_best_perpendicular(const glm::vec3& vector)
{
  return find_best_perpendicular(vector, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
}

inline glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal)
{
  return glm::cross(vector, normal);
}

template<typename... args>
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal_candidate_1 , const glm::vec3& normal_candidate_2, const args&... other_candidates)
{
  if(glm::abs(glm::dot(vector, normal_candidate_1)) <= glm::abs(glm::dot(vector, normal_candidate_2)))
  {
    return find_best_perpendicular(vector, normal_candidate_1, other_candidates...);
  }else
  {
    return find_best_perpendicular(vector, normal_candidate_2, other_candidates...);
  }
}

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


/**
Not as flexible and fancy as glms swizzle operators, but has a much smaller
compile time footprint
*/
inline glm::vec2 xy(glm::vec2 xy)
{
  return glm::vec2(xy);
}

inline glm::vec2 xy(glm::vec3 xyz)
{
  return glm::vec2(xyz);
}

inline glm::vec2 xy(glm::vec4 xyzw)
{
  return glm::vec2(xyzw);
}

inline glm::vec3 xyz(glm::vec3 xyz)
{
  return glm::vec3(xyz);
}

inline glm::vec3 xyz(glm::vec4 xyzw)
{
  return glm::vec3(xyzw);
}
