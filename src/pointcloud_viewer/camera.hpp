#ifndef POINTCLOUDVIEWER_CAMERA_HPP_
#define POINTCLOUDVIEWER_CAMERA_HPP_

#include <geometry/frame.hpp>
#include <geometry/ray.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

/*
Object storing the camera parameters and returning the camera matrices
*/
class Camera final
{
public:
  frame_t frame = frame_t(default_camera_position(),
                          remove_tilt(glm::quatLookAt(glm::normalize(-default_camera_position()), glm::vec3(0,0,1))));
  float aspect = 1.f;
  float fov_y = glm::radians(90.f);
  float z_near = 1.e-3f;
  float z_far = 100.f;

  Camera();
  ~Camera();

  glm::mat4 perspective_matrix() const;
  glm::mat4 view_matrix() const;
  glm::mat4 view_perspective_matrix() const;

  ray_t ray_for_screenspace_point(glm::vec2 screenspace_point) const;
  ray_t ray_for_clipspace_point(glm::vec2 clipspace_point) const;

  static glm::vec2 screenspace_to_clipspace(glm::vec2 screenspace_point);
  static glm::vec2 pixel_to_screenspace(glm::ivec2 pixel, glm::ivec2 viewport_size);

  static glm::vec2 clipspace_screenspace(glm::vec2 clipspace_point);
  static glm::vec2 screenspace_to_pixel(glm::vec2 screenspace_point, glm::ivec2 viewport_size);

private:
  static glm::vec3 default_camera_position()
  {
    return glm::vec3(7.5f, -6.5f, 5.3f) * 0.25f;
  }
};

#endif // POINTCLOUDVIEWER_CAMERA_HPP_
