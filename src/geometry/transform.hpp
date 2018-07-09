#ifndef GEOMETRY_TRANSFORM_HPP_
#define GEOMETRY_TRANSFORM_HPP_

#include <glm/glm.hpp>

// Simple point transformation.
glm::vec3 transform_point(const glm::mat4& t, const glm::vec3& point);
glm::vec3 transform_point(const glm::mat4x3& t, const glm::vec3& point);

// Simple vector transformation.
glm::vec3 transform_direction(const glm::mat4& t, const glm::vec3& relative);
glm::vec3 transform_direction(const glm::mat4x3& t, const glm::vec3& relative);

#include <geometry/transform.inl>

#endif // GEOMETRY_TRANSFORM_HPP_

