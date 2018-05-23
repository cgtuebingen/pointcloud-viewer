#ifndef CORELIBRARY_GEOMETRY_HPP_
#define CORELIBRARY_GEOMETRY_HPP_

#include <glm/glm.hpp>

glm::vec3 find_best_perpendicular(const glm::vec3& vector);
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal);
template<typename... args>
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal_candidate_1 , const glm::vec3& normal_candidate_2, const args&... other_candidates);

glm::vec3 transform_point(const glm::mat4& t, const glm::vec3& point);
glm::vec3 transform_point(const glm::mat4x3& t, const glm::vec3& point);

glm::vec3 transform_direction(const glm::mat4& t, const glm::vec3& relative);
glm::vec3 transform_direction(const glm::mat4x3& t, const glm::vec3& relative);

glm::vec2 xy(glm::vec2 xy);
glm::vec2 xy(glm::vec3 xyz);
glm::vec2 xy(glm::vec4 xyzw);
glm::vec3 xyz(glm::vec3 xyz);
glm::vec3 xyz(glm::vec4 xyzw);

#include <geometry/geometry.inl>

#endif // CORELIBRARY_GEOMETRY_HPP_

