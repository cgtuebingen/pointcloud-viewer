#ifndef RENDERSYSTEM_GL450_LOCATE_SHADERS_HPP_
#define RENDERSYSTEM_GL450_LOCATE_SHADERS_HPP_

#include <renderer/gl450/declarations.hpp>

namespace renderer {
namespace gl450 {

// Call once right after QApplication was initialized to locate the directory containing the shaders
void locate_shaders();

} //namespace gl450
} //namespace renderer

#endif // RENDERSYSTEM_GL450_POINT_RENDERER_HPP_
