#version 450 core

#include <uniforms/global.vs.glsl>

layout(location = POSITION_BINDING_INDEX)
in vec4 point_coord;

void main()
{
  gl_Position = global.camera_matrix * vec4(point_coord.xyz, 1);
}
