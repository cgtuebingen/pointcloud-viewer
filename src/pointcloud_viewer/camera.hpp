#ifndef POINTCLOUDVIEWER_CAMERA_HPP_
#define POINTCLOUDVIEWER_CAMERA_HPP_

#include <geometry/frame.hpp>

class Camera final
{
public:
  frame_t frame = frame_t(glm::vec3(7.481132f, -6.50764f, 5.343665f),
                          glm::quat(0.7816f, 0.481707f, 0.212922f, 0.334251f));
  float fov_y = glm::radians(90.f);
  float near = 1.e-2f;
  float far = 100.f;

  Camera();
  ~Camera();
};

#endif // POINTCLOUDVIEWER_CAMERA_HPP_
