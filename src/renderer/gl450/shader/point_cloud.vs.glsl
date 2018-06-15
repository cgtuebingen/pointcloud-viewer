#version 450 core

#include <uniforms/global.vs.glsl>

layout(location = POSITION_BINDING_INDEX)
in vec3 point_coord;
layout(location = COLOR_BINDING_INDEX)
in vec3 point_color;

out vec4 color;

void main()
{
  gl_Position = global.camera_matrix * vec4(point_coord.xyz, 1);
  
  color = vec4(point_color.rgb, 1);
}
