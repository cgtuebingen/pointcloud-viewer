#ifndef GEOMETRY_SWIZZLE_HPP_
#define GEOMETRY_SWIZZLE_HPP_

#include <glm/glm.hpp>

/**
Not as flexible and fancy as glms swizzle operators, but has a much smaller
compile time footprint
*/

glm::vec2 xy(glm::vec2 xy);
glm::vec2 xy(glm::vec3 xyz);
glm::vec2 xy(glm::vec4 xyzw);
glm::vec3 xyz(glm::vec3 xyz);
glm::vec3 xyz(glm::vec4 xyzw);

#include <geometry/swizzle.inl>

#endif // GEOMETRY_SWIZZLE_HPP_

