#include <pointcloud_viewer/visualizations.hpp>
#include <core_library/color_palette.hpp>

Visualization::Visualization()
  : world_axis(DebugMesh::axis()),
    world_grid(DebugMesh::grid(5, 0.2f, color_palette::grey[1]))
{
}

void Visualization::render()
{
  debug_mesh_renderer.begin();
  debug_mesh_renderer.render(world_grid);
  debug_mesh_renderer.render(world_axis);
  debug_mesh_renderer.end();
}
