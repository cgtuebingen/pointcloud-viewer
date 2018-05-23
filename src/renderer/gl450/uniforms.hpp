#ifndef RENDERSYSTEM_GL450_UNIFORMS_HPP_
#define RENDERSYSTEM_GL450_UNIFORMS_HPP_

#include <renderer/gl450/declarations.hpp>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

namespace renderer {
namespace gl450 {

/**
Classes for representing and managing uniforms
*/

enum uniform_bindings
{
  GLOBAL_UNIFORM = 0,
};

// Stores the global uniforms used by all shaders
class GlobalUniform final
{
public:
  struct vertex_data_t
  {
    glm::mat4 camera_matrix;
  };

  GlobalUniform();
  ~GlobalUniform();

  GlobalUniform(GlobalUniform&& other);
  GlobalUniform& operator=(GlobalUniform&& other);

  void write(vertex_data_t vertex_data);

  void bind();
  void unbind();

private:
  gl::Buffer vertex_uniform_buffer;
};

} //namespace gl450
} //namespace renderer

#endif // RENDERSYSTEM_GL450_UNIFORMS_HPP_
