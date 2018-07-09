#include <renderer/gl450/uniforms.hpp>

namespace renderer {
namespace gl450 {

GlobalUniform::GlobalUniform()
  : vertex_uniform_buffer(sizeof(vertex_data_t), gl::Buffer::UsageFlag::MAP_WRITE, nullptr)
{
}

GlobalUniform::~GlobalUniform()
{
}

GlobalUniform::GlobalUniform(GlobalUniform&& other)
  : vertex_uniform_buffer(std::move(other.vertex_uniform_buffer))
{
}

GlobalUniform& GlobalUniform::operator=(GlobalUniform&& other)
{
  vertex_uniform_buffer = std::move(other.vertex_uniform_buffer);

  return *this;
}

void GlobalUniform::write(GlobalUniform::vertex_data_t vertex_data)
{
  vertex_data_t& _vertex_data = *reinterpret_cast<vertex_data_t*>(vertex_uniform_buffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));

  _vertex_data = vertex_data;

  vertex_uniform_buffer.Unmap();
}

void GlobalUniform::bind()
{
  vertex_uniform_buffer.BindUniformBuffer(GLOBAL_UNIFORM);
}

void GlobalUniform::unbind()
{
}

} //namespace gl450
} //namespace renderer
