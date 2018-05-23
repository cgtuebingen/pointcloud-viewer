#include <geometry/frame.hpp>
#include <geometry/swizzle.hpp>

#include <glm/gtx/quaternion.hpp>

inline void frame_t::_concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                     const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                                     const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor)
{
  // Do the positition first, because it's possible, that &aScaleFactor==outScaleFactor and &aScaleFactor==outOrientation.
  // By calculating the position first the original values are used.
  *outPosition = aPosition + aScaleFactor * (aOrientation * bPosition);
  *outScaleFactor = aScaleFactor * bScaleFactor;
  *outOrientation = aOrientation * bOrientation;
}

inline void frame_t::_coordinate_from_matrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                             glm::mat4 transform)
{
  transform[0][3] = 0;
  transform[1][3] = 0;
  transform[2][3] = 0;
  transform[3][3] = 1;
  glm::vec3 scale(glm::length(xyz(transform[0])),
                  glm::length(xyz(transform[1])),
                  glm::length(xyz(transform[2])));
  *outScaleFactor = (scale.x + scale.y + scale.z) / 3.f;
  transform[0] /= scale.x;
  transform[1] /= scale.y;
  transform[2] /= scale.z;
  *outPosition = xyz(transform[3]);
  transform[3] = glm::vec4(0);
  *outOrientation = glm::quat(transform);
}

inline void frame_t::_transform_point(glm::vec3* outPoint,
                                         const glm::vec3& position, const glm::quat& orientation, float scaleFactor,
                                         const glm::vec3& inPoint)
{
  *outPoint = position + scaleFactor * (orientation * inPoint);
}

inline void frame_t::_transform_direction(glm::vec3* outDirection,
                                             const glm::quat& orientation,
                                             const glm::vec3& inDirection)
{
  *outDirection = orientation * inDirection;
}


inline void frame_t::_to_mat4(float* outMat,
                                 const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor)
{
  const glm::mat3 m = glm::toMat3(inOrientation);

  const float* rot_matrix = reinterpret_cast<const float*>(&m);
  const float* translation = reinterpret_cast<const float*>(&inPosition);

  outMat[0]  = rot_matrix[0] * inScaleFactor;
  outMat[1]  = rot_matrix[1] * inScaleFactor;
  outMat[2]  = rot_matrix[2] * inScaleFactor;
  outMat[3]  = 0.f;
  outMat[4]  = rot_matrix[3] * inScaleFactor;
  outMat[5]  = rot_matrix[4] * inScaleFactor;
  outMat[6]  = rot_matrix[5] * inScaleFactor;
  outMat[7]  = 0.f;
  outMat[8]  = rot_matrix[6] * inScaleFactor;
  outMat[9]  = rot_matrix[7] * inScaleFactor;
  outMat[10] = rot_matrix[8] * inScaleFactor;
  outMat[11] = 0.f;
  outMat[12] = translation[0];
  outMat[13] = translation[1];
  outMat[14] = translation[2];
  outMat[15] = 1.f;
}


inline void frame_t::_to_mat4x3(float* outMat,
                                  const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor)
{
  const glm::mat3 m = glm::toMat3(inOrientation);

  const float* rot_matrix = reinterpret_cast<const float*>(&m);
  const float* translation = reinterpret_cast<const float*>(&inPosition);

  outMat[0]  = rot_matrix[0] * inScaleFactor;
  outMat[1]  = rot_matrix[1] * inScaleFactor;
  outMat[2]  = rot_matrix[2] * inScaleFactor;
  outMat[3]  = rot_matrix[3] * inScaleFactor;
  outMat[4]  = rot_matrix[4] * inScaleFactor;
  outMat[5]  = rot_matrix[5] * inScaleFactor;
  outMat[6]  = rot_matrix[6] * inScaleFactor;
  outMat[7]  = rot_matrix[7] * inScaleFactor;
  outMat[8]  = rot_matrix[8] * inScaleFactor;
  outMat[9]  = translation[0];
  outMat[10] = translation[1];
  outMat[11] = translation[2];
}


inline void frame_t::_inverse(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                                 const glm::vec3& inPosition, const glm::quat& inOrientation, float inScaleFactor)
{
  *outScaleFactor = 1.f/inScaleFactor;
  *outOrientation = glm::inverse(inOrientation);
  *outPosition = - *outScaleFactor * (*outOrientation * inPosition);
}
