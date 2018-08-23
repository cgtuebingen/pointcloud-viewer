#include <pointcloud_viewer/camera.hpp>
#include <geometry/transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::perspective_matrix() const
{
  return glm::perspective(fov_y, aspect, z_near, z_far);
}

glm::mat4 Camera::view_matrix() const
{
  return frame.inverse().to_mat4();
}

glm::mat4 Camera::view_perspective_matrix() const
{
  return perspective_matrix() * view_matrix();
}

ray_t Camera::ray_for_screenspace_point(glm::vec2 screenspace_point) const
{
  return ray_for_clipspace_point(screenspace_to_clipspace(screenspace_point));
}

ray_t Camera::ray_for_clipspace_point(glm::vec2 clipspace_point) const
{
  const glm::mat4 inverse_perspective_matrix = glm::inverse(perspective_matrix());
  const glm::vec3 view_space = glm::normalize(transform_point(inverse_perspective_matrix, glm::vec3(clipspace_point, -1.f)));
  const glm::vec3 world_space = frame * view_space;

  return ray_t::from_two_points(frame.position, world_space);
}

glm::vec2 Camera::screenspace_to_clipspace(glm::vec2 screenspace_point)
{
  return screenspace_point * glm::vec2(2.f, -2.f) + glm::vec2(-1.f, +1.f);
}

glm::vec2 Camera::pixel_to_screenspace(glm::ivec2 screenspace_pixel, glm::ivec2 viewport_size)
{
  return glm::vec2(screenspace_pixel) / glm::vec2(viewport_size - 1);
}
