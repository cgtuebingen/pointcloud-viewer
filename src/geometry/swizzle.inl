#include <geometry/swizzle.hpp>

inline glm::vec2 xy(glm::vec2 xy)
{
  return glm::vec2(xy);
}

inline glm::vec2 xy(glm::vec3 xyz)
{
  return glm::vec2(xyz);
}

inline glm::vec2 xz(glm::vec3 xyz)
{
  return glm::vec2(xyz.x, xyz.z);
}

inline glm::vec2 yz(glm::vec3 xyz)
{
  return glm::vec2(xyz.y, xyz.z);
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
