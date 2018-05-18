#ifndef POINTCLOUDVIEWER_VISUALIZATIONS_HPP_
#define POINTCLOUDVIEWER_VISUALIZATIONS_HPP_

#include <renderer/gl450/point_renderer.hpp>
#include <renderer/gl450/debug/debug_mesh.hpp>

/**
Class responsible for rendering visualizations

- world axis
- world grid
*/
class Visualization
{
public:
  Visualization();

  void render();

private:
  typedef render_system::gl450::DebugMeshRenderer DebugMeshRenderer;
  typedef render_system::gl450::DebugMesh DebugMesh;

  DebugMeshRenderer debug_mesh_renderer;

  DebugMesh world_axis;
};

#endif // POINTCLOUDVIEWER_VISUALIZATIONS_HPP_
