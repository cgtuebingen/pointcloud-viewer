#ifndef POINTCLOUDVIEWER_VISUALIZATIONS_HPP_
#define POINTCLOUDVIEWER_VISUALIZATIONS_HPP_

#include <renderer/gl450/point_renderer.hpp>
#include <renderer/gl450/debug/debug_mesh.hpp>

#include <pointcloud_viewer/flythrough/keypoint.hpp>

/**
Class responsible for rendering visualizations

- world axis
- world grid
*/
class Visualization : public QObject
{
public:
  struct settings_t
  {
    bool enable_any_visualizations : 1;
    bool enable_grid : 1;
    bool enable_axis : 1;
    bool enable_turntable_center : 1;
    bool enable_camera_path : 1;
    bool enable_kdtree_as_aabb : 1;
    bool enable_picked_cone : 1;
    bool enable_selected_point : 1;

    static settings_t enable_all();
    static settings_t default_settings();
    static settings_t disable_all();
  };

  settings_t settings = settings_t::default_settings();

  Visualization();

  void render();

  void set_turntable_origin(glm::vec3 origin);
  void set_path(const QVector<keypoint_t>& keypoints, int selected_point);

  void set_kdtree_as_aabb(aabb_t highlighted_aabb, glm::vec3 separator_point, aabb_t other_aabb);
  void set_picked_cone(cone_t picked_cone);

  void deselect_picked_point();
  void select_picked_point(glm::vec3 coordinate, glm::u8vec3 color, float radius);

private:
  typedef renderer::gl450::DebugMeshRenderer DebugMeshRenderer;
  typedef renderer::gl450::DebugMesh DebugMesh;

  DebugMeshRenderer debug_mesh_renderer;

  DebugMesh world_axis;
  DebugMesh world_grid;
  DebugMesh turntable_origin;
  DebugMesh camera_path;
  DebugMesh picked_cone;
  DebugMesh selected_point;

  DebugMesh kdtree_normal_aabb;
  DebugMesh kdtree_highlight_aabb;
  DebugMesh kdtree_current_point;
};

#endif // POINTCLOUDVIEWER_VISUALIZATIONS_HPP_
