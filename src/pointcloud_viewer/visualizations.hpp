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
  struct settings_t
  {
    bool enable_any_visualizations : 1;
    bool enable_grid : 1;
    bool enable_axis : 1;
    bool enable_turntable_center : 1;

    static settings_t enable_all();
    static settings_t default_settings();
    static settings_t disable_all();
  };

  settings_t settings = settings_t::default_settings();

  Visualization();

  void render();

  void set_turntable_origin(glm::vec3 origin);

private:
  typedef renderer::gl450::DebugMeshRenderer DebugMeshRenderer;
  typedef renderer::gl450::DebugMesh DebugMesh;

  DebugMeshRenderer debug_mesh_renderer;

  DebugMesh world_axis;
  DebugMesh world_grid;
  DebugMesh turntable_origin;
};

#endif // POINTCLOUDVIEWER_VISUALIZATIONS_HPP_
