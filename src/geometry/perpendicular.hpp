#ifndef GEOMETRY_PERPENDICULAR_HPP_
#define GEOMETRY_PERPENDICULAR_HPP_

#include <glm/glm.hpp>

/**
The cross product betwen two vectors always returns a third vector orthogonal to
the plane, where both given vectors lie in.

The find_best_perpendicular finds a vector perpendicular to the give vector. It
checks multiple candidate for the most perpendicular one and then returns the
cross product to that one.

To guarantee a valid result, provide at least two perpendicular vectors, as done
by find_best_perpendicular(vec3), which uses all three axis (vec3(1,0,0),
vec3(0,1,0) and vec3(0,0,1))
*/

glm::vec3 find_best_perpendicular(const glm::vec3& vector);
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal);
template<typename... args>
glm::vec3 find_best_perpendicular(const glm::vec3& vector, const glm::vec3& normal_candidate_1 , const glm::vec3& normal_candidate_2, const args&... other_candidates);

#include <geometry/perpendicular.inl>

#endif // GEOMETRY_PERPENDICULAR_HPP_

