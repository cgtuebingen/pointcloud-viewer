#include <glrt/scene/coord-frame.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

CoordFrame::CoordFrame(glm::ctor ctor)
  : position(ctor),
    orientation(ctor)
{
}

CoordFrame::CoordFrame(const glm::vec3& position, const glm::quat& orientation, float scaleFactor)
  : position(position),
    scaleFactor(scaleFactor),
    orientation(orientation)
{
}

CoordFrame::CoordFrame(const glm::mat4& transformation)
{
  _coordinateFromMatrix(&this->position, &this->orientation, &this->scaleFactor, transformation);
}

CoordFrame::CoordFrame(const aiMatrix4x4& transformation)
  : CoordFrame(to_glm_mat4(transformation))
{
}

CoordFrame& CoordFrame::operator *=(const CoordFrame& other)
{
  _concatenate(&this->position, &this->orientation, &this->scaleFactor, this->position, this->orientation, this->scaleFactor, other.position, other.orientation, other.scaleFactor);
  return *this;
}

CoordFrame CoordFrame::operator *(const CoordFrame& other) const
{
  CoordFrame f = *this;

  return f *= other;
}

glm::vec3 CoordFrame::operator *(const glm::vec3& point) const
{
  return this->transform_point(point);
}

bool CoordFrame::operator==(const CoordFrame& frame) const
{
  return this->position == frame.position
      && this->orientation == frame.orientation
      && this->scaleFactor == frame.scaleFactor;
}

bool CoordFrame::operator!=(const CoordFrame& frame) const
{
  return !(*this == frame);
}

glm::vec3 CoordFrame::transform_point(const glm::vec3& point) const
{
  glm::vec3 transformed_point;
  _transform_point(&transformed_point,
                   this->position, this->orientation, this->scaleFactor,
                   point);
  return transformed_point;
}

glm::vec3 CoordFrame::transform_direction(const glm::vec3& point) const
{
  glm::vec3 transformed_direction;
  _transform_direction(&transformed_direction,
                       this->orientation,
                       point);
  return transformed_direction;
}

glm::mat4 CoordFrame::toMat4() const
{
  glm::mat4 m;

  _to_mat4(reinterpret_cast<float*>(&m),
           this->position, this->orientation, this->scaleFactor);
  return m;
}

glm::mat4x3 CoordFrame::toMat4x3() const
{
  glm::mat4x3 m;

  _to_mat4x3(reinterpret_cast<float*>(&m),
            this->position, this->orientation, this->scaleFactor);
  return m;
}

CoordFrame CoordFrame::inverse() const
{
  CoordFrame i;
  _inverse(&i.position, &i.orientation, &i.scaleFactor,
           this->position, this->orientation, this->scaleFactor);
  return i;
}

QString CoordFrame::as_angelscript_fast() const
{
  return QString("CoordFrame(vec3(%0, %1, %2), quat(%3, %4, %5, %6), %7)").arg(this->position.x).arg(this->position.y).arg(this->position.z).arg(this->orientation.w).arg(this->orientation.x).arg(this->orientation.y).arg(this->orientation.z).arg(this->scaleFactor);
}

QDebug operator<<(QDebug debug, const CoordFrame& coordFrame)
{
  return debug << "CoordFrame(position: " << coordFrame.position << ", orientation: " << coordFrame.orientation << ", scale: " << coordFrame.scaleFactor << ")";
}

std::ostream& operator<<(std::ostream& stream, const CoordFrame& coordFrame)
{
  return stream << "CoordFrame(position: " << coordFrame.position << ", orientation: " << coordFrame.orientation << ", scale: " << coordFrame.scaleFactor << ")";
}

inline void create_CoordFrame(CoordFrame* coordFrame, const glm::vec3& position, const glm::quat& orientation, float scaleFactor)
{
  coordFrame->position = position;
  coordFrame->orientation = orientation;
  coordFrame->scaleFactor = scaleFactor;
}

void CoordFrame::registerAngelScriptAPIDeclarations()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;
  r = angelScriptEngine->RegisterObjectType("CoordFrame",
                                            sizeof(CoordFrame),
                                            AngelScript::asOBJ_VALUE |
                                            AngelScript::asOBJ_POD |
                                            AngelScript::asOBJ_APP_CLASS |
                                            AngelScript::asOBJ_APP_CLASS_CONSTRUCTOR |
                                            AngelScript::asOBJ_APP_CLASS_ASSIGNMENT |
                                            AngelScript::asOBJ_APP_CLASS_COPY_CONSTRUCTOR |
                                            AngelScript::asOBJ_APP_CLASS_ALLFLOATS);
  AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void CoordFrame::registerAngelScriptAPI()
{
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  int r;
  r = angelScriptEngine->RegisterObjectBehaviour("CoordFrame",
                                                 AngelScript::asBEHAVE_CONSTRUCT,
                                                 "void f(const vec3 &in position, const quat &in orientation, float scaleFactor)",
                                                 AngelScript::asFUNCTION(create_CoordFrame),
                                                 AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectBehaviour("CoordFrame",
                                                 AngelScript::asBEHAVE_CONSTRUCT,
                                                 "void f()",
                                                 AngelScript::asFUNCTION(AngelScriptIntegration::wrap_constructor<CoordFrame>),
                                                 AngelScript::asCALL_CDECL_OBJFIRST);
  AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

} // namespace scene
} // namespace glrt

