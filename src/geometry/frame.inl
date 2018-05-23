#include <geometry/frame.hpp>
#include <geometry/swizzle.hpp>

#include <glm/gtx/quaternion.hpp>

inline void frame_t::_concatenate(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                                  const glm::vec3& in_position_1, const glm::quat& in_orientation_1, float in_scale_factor_1,
                                  const glm::vec3& in_position_2, const glm::quat& in_orientation_2, float in_scale_factor_2)
{
  // Do the positition first, because it's possible, that &in_scale_factor_1==out_scale_factor and &in_scale_factor_1==out_orientation.
  // By calculating the position first the original values are used.
  *out_position = in_position_1 + in_scale_factor_1 * (in_orientation_1 * in_position_2);
  *out_scale_factor = in_scale_factor_1 * in_scale_factor_2;
  *out_orientation = in_orientation_1 * in_orientation_2;
}

inline void frame_t::_coordinate_from_matrix(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                                             glm::mat4 transform)
{
  transform[0][3] = 0;
  transform[1][3] = 0;
  transform[2][3] = 0;
  transform[3][3] = 1;
  glm::vec3 scale(glm::length(xyz(transform[0])),
                  glm::length(xyz(transform[1])),
                  glm::length(xyz(transform[2])));
  *out_scale_factor = (scale.x + scale.y + scale.z) / 3.f;
  transform[0] /= scale.x;
  transform[1] /= scale.y;
  transform[2] /= scale.z;
  *out_position = xyz(transform[3]);
  transform[3] = glm::vec4(0);
  *out_orientation = glm::quat(transform);
}

inline void frame_t::_transform_point(glm::vec3* out_point,
                                      const glm::vec3& position, const glm::quat& orientation, float scale_factor,
                                      const glm::vec3& in_point)
{
  *out_point = position + scale_factor * (orientation * in_point);
}

inline void frame_t::_transform_direction(glm::vec3* out_direction,
                                          const glm::quat& orientation,
                                          const glm::vec3& in_direction)
{
  *out_direction = orientation * in_direction;
}


inline void frame_t::_to_mat4(float* out_mat,
                              const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor)
{
  const glm::mat3 m = glm::toMat3(in_orientation);

  const float* rot_matrix = reinterpret_cast<const float*>(&m);
  const float* translation = reinterpret_cast<const float*>(&in_position);

  out_mat[0]  = rot_matrix[0] * in_scale_factor;
  out_mat[1]  = rot_matrix[1] * in_scale_factor;
  out_mat[2]  = rot_matrix[2] * in_scale_factor;
  out_mat[3]  = 0.f;
  out_mat[4]  = rot_matrix[3] * in_scale_factor;
  out_mat[5]  = rot_matrix[4] * in_scale_factor;
  out_mat[6]  = rot_matrix[5] * in_scale_factor;
  out_mat[7]  = 0.f;
  out_mat[8]  = rot_matrix[6] * in_scale_factor;
  out_mat[9]  = rot_matrix[7] * in_scale_factor;
  out_mat[10] = rot_matrix[8] * in_scale_factor;
  out_mat[11] = 0.f;
  out_mat[12] = translation[0];
  out_mat[13] = translation[1];
  out_mat[14] = translation[2];
  out_mat[15] = 1.f;
}


inline void frame_t::_to_mat4x3(float* out_mat,
                                  const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor)
{
  const glm::mat3 m = glm::toMat3(in_orientation);

  const float* rot_matrix = reinterpret_cast<const float*>(&m);
  const float* translation = reinterpret_cast<const float*>(&in_position);

  out_mat[0]  = rot_matrix[0] * in_scale_factor;
  out_mat[1]  = rot_matrix[1] * in_scale_factor;
  out_mat[2]  = rot_matrix[2] * in_scale_factor;
  out_mat[3]  = rot_matrix[3] * in_scale_factor;
  out_mat[4]  = rot_matrix[4] * in_scale_factor;
  out_mat[5]  = rot_matrix[5] * in_scale_factor;
  out_mat[6]  = rot_matrix[6] * in_scale_factor;
  out_mat[7]  = rot_matrix[7] * in_scale_factor;
  out_mat[8]  = rot_matrix[8] * in_scale_factor;
  out_mat[9]  = translation[0];
  out_mat[10] = translation[1];
  out_mat[11] = translation[2];
}


inline void frame_t::_inverse(glm::vec3* out_position, glm::quat* out_orientation, float* out_scale_factor,
                                 const glm::vec3& in_position, const glm::quat& in_orientation, float in_scale_factor)
{
  *out_scale_factor = 1.f/in_scale_factor;
  *out_orientation = glm::inverse(in_orientation);
  *out_position = - *out_scale_factor * (*out_orientation * in_position);
}
