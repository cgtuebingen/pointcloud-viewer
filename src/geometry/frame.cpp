#include <geometry/frame.hpp>
#include <glm/gtx/io.hpp>

frame_t::frame_t(const glm::vec3& position, const glm::quat& orientation, float scaleFactor)
  : position(position),
    scaleFactor(scaleFactor),
    orientation(orientation)
{
}

frame_t::frame_t(const glm::mat4& transformation)
{
  _coordinate_from_matrix(&this->position, &this->orientation, &this->scaleFactor, transformation);
}

frame_t& frame_t::operator *=(const frame_t& other)
{
  _concatenate(&this->position, &this->orientation, &this->scaleFactor, this->position, this->orientation, this->scaleFactor, other.position, other.orientation, other.scaleFactor);
  return *this;
}

frame_t frame_t::operator *(const frame_t& other) const
{
  frame_t f = *this;

  return f *= other;
}

glm::vec3 frame_t::operator *(const glm::vec3& point) const
{
  return this->transform_point(point);
}

glm::vec3 frame_t::transform_point(const glm::vec3& point) const
{
  glm::vec3 transformed_point;
  _transform_point(&transformed_point,
                   this->position, this->orientation, this->scaleFactor,
                   point);
  return transformed_point;
}

glm::vec3 frame_t::transform_direction(const glm::vec3& point) const
{
  glm::vec3 transformed_direction;
  _transform_direction(&transformed_direction,
                       this->orientation,
                       point);
  return transformed_direction;
}

glm::mat4 frame_t::to_mat4() const
{
  glm::mat4 m;

  _to_mat4(reinterpret_cast<float*>(&m),
           this->position, this->orientation, this->scaleFactor);
  return m;
}

glm::mat4x3 frame_t::to_mat_4x3() const
{
  glm::mat4x3 m;

  _to_mat4x3(reinterpret_cast<float*>(&m),
            this->position, this->orientation, this->scaleFactor);
  return m;
}

frame_t frame_t::inverse() const
{
  frame_t i;
  _inverse(&i.position, &i.orientation, &i.scaleFactor,
           this->position, this->orientation, this->scaleFactor);
  return i;
}

std::ostream& operator<<(std::ostream& stream, const frame_t& coordFrame)
{
  return stream << "CoordFrame(position: " << coordFrame.position << ", orientation: " << coordFrame.orientation << ", scale: " << coordFrame.scaleFactor << ")";
}

inline void create_CoordFrame(frame_t* coordFrame, const glm::vec3& position, const glm::quat& orientation, float scaleFactor)
{
  coordFrame->position = position;
  coordFrame->orientation = orientation;
  coordFrame->scaleFactor = scaleFactor;
}
