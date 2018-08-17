#ifndef GEOMETRY_AABB_HPP_
#define GEOMETRY_AABB_HPP_

#include <geometry/frame.hpp>
#include <core_library/padding.hpp>

struct aabb_t
{
  glm::vec3 min_point;
  padding<float, 1> _padding1;
  glm::vec3 max_point;
  padding<float, 1> _padding2;

  bool is_inf() const {return glm::any(glm::isinf(min_point)) || glm::any(glm::isinf(max_point));}
  bool is_nan() const {return glm::any(glm::isnan(min_point)) || glm::any(glm::isnan(max_point));}
  bool is_valid() const{return !is_inf() && !is_nan() && all(greaterThan(max_point, min_point));}

  glm::vec3 toUnitSpace(const glm::vec3& v) const;

  glm::vec3 center_point() const;
  glm::vec3 size() const;

  void operator |= (const aabb_t& other);
  void operator |= (const glm::vec3& other);

  static aabb_t invalid();

  static aabb_t fromVertices(const glm::vec3* vertices, int num_vertices);
  static aabb_t fromVertices(const glm::vec3* vertices, int num_vertices, size_t stride);
  aabb_t aabbOfTransformedBoundingBox(const frame_t& coordFrame) const;

  aabb_t ensureValid() const;
};

std::ostream& operator<<(std::ostream& stream, const aabb_t& aabb);

#endif // GEOMETRY_AABB_HPP_
