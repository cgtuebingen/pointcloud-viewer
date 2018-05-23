#version 450 core

layout(location = POSITION_BINDING_INDEX)
in vec4 point_coord;

void main()
{
  gl_Position = vec4(point_coord.xyz, 1);
}
