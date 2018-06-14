#include <renderer/gl450/point_renderer.hpp>

#include <glm/gtc/constants.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

namespace renderer {
namespace gl450 {

const int NUM_VERTICES = 512;

const int STRIDE = 4*4;

const int POSITION_BINDING_INDEX = 0;
const int COLOR_BINDING_INDEX = 1;
const int PADDING = 0;


PointRenderer::PointRenderer()
  : shader_object("point_renderer"),
    vertex_position_buffer(NUM_VERTICES * sizeof(glm::vec4), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    vertex_array_object({gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::FLOAT, 3, POSITION_BINDING_INDEX),
                        gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::UINT8, 3, COLOR_BINDING_INDEX),
})
{
  shader_object.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX,
                                  "point_cloud.vs.glsl",
                                  format("#define POSITION_BINDING_INDEX ", POSITION_BINDING_INDEX, "\n",
                                         "#define COLOR_BINDING_INDEX ", COLOR_BINDING_INDEX, "\n"));
  shader_object.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT,
                                  "point_cloud.fs.glsl");
  shader_object.CreateProgram();

  glm::vec4* vertex = reinterpret_cast<glm::vec4*>(vertex_position_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  for(int i=0; i<NUM_VERTICES; ++i)
  {
    float32_t color;
    write_value_to_buffer<uint32_t>(&color, 0x00ffffff); TODO wha is the point still not white?

    float angle = glm::two_pi<float>() * i / float(NUM_VERTICES);
    vertex[i] = glm::vec4(glm::cos(angle), glm::sin(angle), 0.f, color);
  }
  vertex_position_buffer.Unmap();
  vertex = nullptr;
}

PointRenderer::~PointRenderer()
{
}

PointRenderer::PointRenderer(PointRenderer&& point_renderer)
  : shader_object(std::move(point_renderer.shader_object)),
    vertex_position_buffer(std::move(point_renderer.vertex_position_buffer)),
    vertex_array_object(std::move(point_renderer.vertex_array_object))
{
}

PointRenderer& PointRenderer::operator=(PointRenderer&& point_renderer)
{
  shader_object = std::move(point_renderer.shader_object);
  vertex_position_buffer = std::move(point_renderer.vertex_position_buffer);
  vertex_array_object = std::move(point_renderer.vertex_array_object);
  return *this;
}

void PointRenderer::render_points()
{
  vertex_array_object.Bind();
  vertex_position_buffer.BindVertexBuffer(POSITION_BINDING_INDEX, 0, STRIDE);
  vertex_position_buffer.BindVertexBuffer(COLOR_BINDING_INDEX, 3*4, STRIDE);

  shader_object.Activate();
  GL_CALL(glDrawArrays, GL_POINTS, 0, NUM_VERTICES);
  shader_object.Deactivate();
}

} //namespace gl450
} //namespace renderer
