#ifndef GLRT_SCENE_COORDFRAME_H
#define GLRT_SCENE_COORDFRAME_H

#include <glrt/dependencies.h>

#include <glrt/toolkit/assimp-glm-converter.h>

namespace glrt {
namespace scene {

struct CoordFrame final
{
  glm::vec3 position;
  float scaleFactor;
  glm::quat orientation;

  CoordFrame(const glm::ctor);
  explicit CoordFrame(const glm::vec3& position = glm::vec3(0),
                      const glm::quat& orientation = glm::quat::IDENTITY,
                      float scaleFactor = 1.f);
  CoordFrame(const glm::mat4& transformation);
  CoordFrame(const aiMatrix4x4& transformation);

  CoordFrame& operator *=(const CoordFrame& other);

  CoordFrame operator *(const CoordFrame& other) const;
  glm::vec3 operator *(const glm::vec3& point) const;

  bool operator==(const CoordFrame& frame) const;
  bool operator!=(const CoordFrame& frame) const;

  glm::vec3 transform_point(const glm::vec3& point) const;
  glm::vec3 transform_direction(const glm::vec3& point) const;

  glm::mat4x3 toMat4x3() const;
  glm::mat4 toMat4() const;
  CoordFrame inverse() const;

  QString as_angelscript_fast() const;

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  static void _concatenate(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
                           const glm::vec3& aPosition, const glm::quat& aOrientation, float aScaleFactor,
                           const glm::vec3& bPosition, const glm::quat& bOrientation, float bScaleFactor);
  static void _coordinateFromMatrix(glm::vec3* outPosition, glm::quat* outOrientation, float* outScaleFactor,
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

QDebug operator<<(QDebug debug, const CoordFrame& coordFrame);
std::ostream& operator<<(std::ostream& stream, const CoordFrame& coordFrame);

inline uint qHash(const CoordFrame& frame, uint seed=0)
{
  return ::qHash(frame.position, ::qHash(frame.scaleFactor, ::qHash(frame.orientation, seed)));
}

} // namespace scene
} // namespace glrt

#include "coord-frame.inl"

#endif // GLRT_SCENE_COORDFRAME_H
