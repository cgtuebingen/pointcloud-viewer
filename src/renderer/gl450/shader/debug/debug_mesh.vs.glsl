#version 450 core

#include <uniforms/global.vs.glsl>

layout(location = DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION)
in vec3 vertex_position;

layout(location = DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR)
in vec3 vertex_color;

out vec3 color;

void main()
{
  gl_Position = global.camera_matrix * vec4(vertex_position.xyz, 1);
  color = vertex_color;
}
