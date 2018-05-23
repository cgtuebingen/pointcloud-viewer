struct GlobalUniform
{
  mat4 camera_matrix;
};

layout(binding=0, std140) uniform GlobalBlock
{
  GlobalUniform global;
};
