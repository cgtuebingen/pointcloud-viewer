#include <core_library/color_palette.hpp>
#include <core_library/print.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <geometry/transform.hpp>

#include <stdio.h>

#include <QPainter>

Visualization::Visualization()
  : world_axis(DebugMesh::axis()),
    world_grid(DebugMesh::grid(5, 1.f, color_palette::grey[1])),
    turntable_origin(DebugMesh::turntable_point(glm::vec3(0))),
    trackball(DebugMesh::trackball(glm::vec3(0))),
    camera_path(DebugMesh::path(0, [](int)->frame_t{Q_UNREACHABLE();}, -1))
{
}

void Visualization::render()
{
  if(!settings.enable_any_visualizations)
    return;

  debug_mesh_renderer.begin();
  if(settings.enable_grid)
    debug_mesh_renderer.render(world_grid);
  if(settings.enable_axis)
    debug_mesh_renderer.render(world_axis);
  if(settings.enable_turntable_center)
    debug_mesh_renderer.render(turntable_origin);
  if(settings.enable_trackball)
    debug_mesh_renderer.render(trackball);
  if(settings.enable_camera_path)
    debug_mesh_renderer.render(camera_path);
  if(settings.enable_picked_cone)
    debug_mesh_renderer.render(picked_cone);
  if(settings.enable_kdtree_as_aabb)
  {
    debug_mesh_renderer.render(kdtree_current_point);
    debug_mesh_renderer.render(kdtree_normal_aabb);
    debug_mesh_renderer.render(kdtree_highlight_aabb);
  }
  debug_mesh_renderer.end();
}

void Visualization::set_turntable_origin(glm::vec3 origin)
{
  turntable_origin = DebugMesh::turntable_point(origin);
}

void Visualization::set_path(const QVector<keypoint_t>& keypoints, int selected_point)
{
  camera_path = DebugMesh::path(keypoints.length(), [&keypoints](int i){return keypoints[i].frame;}, selected_point);
}

void Visualization::set_kdtree_as_aabb(aabb_t highlighted_aabb, glm::vec3 separator_point, aabb_t other_aabb)
{
  glm::vec3 normal_color = glm::vec3(1,0.5,0);
  glm::vec3 highlight_color = glm::vec3(1,1,0.2);

  kdtree_current_point = DebugMesh::turntable_point(separator_point, 0.25f, normal_color);
  kdtree_normal_aabb = DebugMesh::aabb(other_aabb, normal_color);
  kdtree_highlight_aabb = DebugMesh::aabb(highlighted_aabb, highlight_color);
}

void Visualization::set_picked_cone(cone_t picked_cone)
{
  this->picked_cone = DebugMesh::cone(picked_cone);
}

void Visualization::deselect_picked_point()
{
  this->has_selected_point = false;
}

void Visualization::select_picked_point(glm::vec3 coordinate, glm::u8vec3 color)
{
  this->has_selected_point = true;
  this->selected_point_coordinate = coordinate;
  this->selected_point_color = color;
}

void Visualization::draw_overlay(QPainter& painter, const Camera& camera, int pointSize, glm::ivec2 viewport_size)
{
  if(this->has_selected_point)
  {
    const glm::mat4 view_perspective_matrix = camera.view_perspective_matrix();
    const glm::vec3 selected_point = transform_point(view_perspective_matrix, this->selected_point_coordinate);

    if(selected_point.z > 1.f)
      return;

    glm::ivec2 selected_point_pixel = glm::ivec2(glm::round(Camera::screenspace_to_pixel(Camera::clipspace_screenspace(selected_point), viewport_size)));

    const bool is_bright_color = (selected_point_color.x + selected_point_color.y + selected_point_color.z) / 2 > 196;

    QRect selection_box = QRect(selected_point_pixel.x-(pointSize)/2-2, selected_point_pixel.y-(pointSize)/2-2, pointSize+2, pointSize+2);

    painter.setBrush(QBrush(Color(selected_point_color)));
    painter.setPen(QColor::fromRgb(is_bright_color ? 0x000000 : 0xffffff));
    painter.drawRect(selection_box);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QColor::fromRgb(is_bright_color ? 0xffffff : 0x000000));
    selection_box.adjust(-1, -1, 1, 1);
    painter.drawRect(selection_box);
  }
}

void Visualization::set_trackball(glm::vec3 center, float radius)
{
  trackball = DebugMesh::trackball(center, radius);
}

Visualization::settings_t Visualization::settings_t::enable_all()
{
  settings_t settings;

  memset(&settings, 0xff, sizeof(settings));

  return settings;
}

Visualization::settings_t Visualization::settings_t::default_settings()
{
  settings_t settings = enable_all();

  settings.enable_turntable_center = false;
  settings.enable_kdtree_as_aabb = false;
  settings.enable_picked_cone = false;
  settings.enable_trackball = false;
  settings.enable_grid = false;

  return settings;
}

Visualization::settings_t Visualization::settings_t::disable_all()
{
  settings_t settings;

  memset(&settings, 0, sizeof(settings));

  return settings;
}
