#include <pointcloud_viewer/visualizations.hpp>

Visualization::Visualization()
  : world_axis(DebugMesh::axis())
{
}

void Visualization::render()
{
  debug_mesh_renderer.begin();
  debug_mesh_renderer.render(world_axis);
  debug_mesh_renderer.end();
}
