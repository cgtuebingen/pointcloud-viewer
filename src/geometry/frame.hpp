#ifndef GEOMETRY_FRAME_HPP_
#define GEOMETRY_FRAME_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

struct frame_t final
{
  glm::vec3 position;
  float scaleFactor;
  glm::quat orientation;

  explicit frame_t(const glm::vec3& position = glm::vec3(),
                      const glm::quat& orientation = glm::quat(),
                      float scaleFactor = 1.f);
  frame_t(const glm::mat4& transformation);

  frame_t& operator *=(const frame_t& other);

  frame_t operator *(const frame_t& other) const;
  glm::vec3 operator *(const glm::vec3& point) const;

  glm::vec3 transform_point(const glm::vec3& point) const;
  glm::vec3 transform_direction(const glm::vec3& point) const;

  glm::mat4x3 to_mat_4x3() const;
  glm::mat4 to_mat4() const;
  frame_t inverse() const;

  static void _concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                           const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                           const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor);
  static void _coordinate_from_matrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                    glm::mat4 transform);
  static void _transform_point(glm::vec3* outPoint,
                               const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                               const glm::vec3& inPoint);
  static void _transform_direction(glm::vec3* outDirection,
                                   const glm::quat& orientation,
                                   const glm::vec3& inDirection);
  static void _to_mat4x3(float* outMat,
                        const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor);
  static void _to_mat4(float* outMat,
                       const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor);
  static void _inverse(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                       const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor);
};

std::ostream& operator<<(std::ostream& stream, const frame_t& coordFrame);

#include <geometry/frame.inl>

#endif // GEOMETRY_FRAME_HPP_
