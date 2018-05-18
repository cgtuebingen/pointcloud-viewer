#include <core_library/geometry.hpp>

template<typename T>
inline T voxelIndexForCoordinate(const glm::tvec3<T>& voxelCoordinate, const glm::tvec3<T>& gridSize)
{
  return voxelCoordinate.z * gridSize.y * gridSize.x + voxelCoordinate.y * gridSize.x + voxelCoordinate.x;
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

inline glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal)
{
  return glm::cross(vector, normal);
}

inline glm::vec3 find_best_perpendicular(const glm::vec3& vector)
{
  return find_best_perpendicular(vector, glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0));
}


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
