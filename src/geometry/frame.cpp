#include <geometry/frame.hpp>
#include <glm/gtx/io.hpp>

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
