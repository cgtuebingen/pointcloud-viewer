#include <pointcloud_viewer/camera.hpp>

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
