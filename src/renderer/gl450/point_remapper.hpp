#ifndef RENDERSYSTEM_GL450_POINT_REMAPPER_HPP_
#define RENDERSYSTEM_GL450_POINT_REMAPPER_HPP_

#include <renderer/gl450/declarations.hpp>
#include <pointcloud/pointcloud.hpp>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

namespace renderer {
namespace gl450 {

/**
Remaps the point coordinates and colors osed for rendering
*/
void remap_points(const std::string& vertex_shader, const QVector<uint>& bindings, PointCloud* pointCloud);

} //namespace gl450
} //namespace renderer

#endif // RENDERSYSTEM_GL450_POINT_RENDERER_HPP_
