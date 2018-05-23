#ifndef GEOMETRY_FRAME_HPP_
#define GEOMETRY_FRAME_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

template<typename quat_type>
inline quat_type quat_identity();

struct frame_t final
{
  glm::vec3 position;
  float scale_factor;
  glm::quat orientation;

  frame_t(){}
  explicit frame_t(const glm::vec3& position,
                   const glm::quat& orientation = quat_identity<glm::quat>(),
                   float scale_factor = 1.f);
  frame_t(const glm::mat4& transformation);

  frame_t& operator *=(const frame_t& other);

  frame_t operator *(const frame_t& other) const;
  glm::vec3 operator *(const glm::vec3& point) const;

  glm::vec3 transform_point(const glm::vec3& point) const;
  glm::vec3 transform_direction(const glm::vec3& point) const;

  glm::mat4x3 to_mat_4x3() const;
  glm::mat4 to_mat4() const;
  frame_t inverse() const;

  static void _concatenate(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                           const glm::vec3& in_position_1, const glm::quat& in_orientation_1, float in_scale_factor_1,
                           const glm::vec3& in_position_2, const glm::quat& in_orientation_2, float in_scale_factor_2);
  static void _coordinate_from_matrix(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                                      glm::mat4 transform);
  static void _transform_point(glm::vec3* out_point,
                               const glm::vec3& position, const glm::quat& orientation, float scale_factor,
                               const glm::vec3& in_point);
  static void _transform_direction(glm::vec3* out_direction,
                                   const glm::quat& orientation,
                                   const glm::vec3& in_direction);
  static void _to_mat4x3(float* out_mat,
                         const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor);
  static void _to_mat4(float* out_mat,
                       const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor);
  static void _inverse(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                       const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor);
};

std::ostream& operator<<(std::ostream& stream, const frame_t& frame);

#include <geometry/frame.inl>

#endif // GEOMETRY_FRAME_HPP_
