#ifndef POINTCLOUDVIEWER_CAMERA_HPP_
#define POINTCLOUDVIEWER_CAMERA_HPP_

#include <geometry/frame.hpp>
#include <geometry/ray.hpp>

/*
Object storing the camera parameters and returning the camera matrices
*/
class Camera final
{
public:
  frame_t frame = frame_t(glm::vec3(7.481132f, -6.50764f, 5.343665f) * 0.25f,
                          remove_tilt(glm::quat(0.7816f, 0.481707f, 0.212922f, 0.334251f)));
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
};

#endif // POINTCLOUDVIEWER_CAMERA_HPP_
