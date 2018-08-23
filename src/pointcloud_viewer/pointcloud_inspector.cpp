#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud/pointcloud.hpp>
#include <core_library/print.hpp>
#include <glm/gtx/io.hpp>

PointCloudInspector::PointCloudInspector(Viewport* viewport)
  : viewport(*viewport)
{
}

PointCloudInspector::~PointCloudInspector()
{
}

// Called when athe point-cloud was unloaded
void PointCloudInspector::unload_all_point_clouds()
{
  this->point_cloud.clear();
}

// Called when a point-cloud was loaded
void PointCloudInspector::handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;
}

void PointCloudInspector::pick_point(glm::ivec2 pixel)
{
  if(!point_cloud || !point_cloud->has_build_kdtree())
    return;

  float pick_radius = 2.f;
  glm::ivec2 viewport_size(viewport.width(), viewport.height());

  glm::vec2 screenspace_point = Camera::pixel_to_screenspace(pixel, viewport_size);

  ray_t center_ray = viewport.navigation.camera.ray_for_screenspace_point(screenspace_point);
  ray_t side_ray = viewport.navigation.camera.ray_for_screenspace_point(glm::mix(screenspace_point, Camera::pixel_to_screenspace(pixel+glm::ivec2(1, 0), viewport_size), pick_radius));

  float angle = glm::acos(glm::dot(center_ray.direction, side_ray.direction));

  cone_t cone = cone_t::cone_from_ray_angle(center_ray, angle);

  viewport.visualization().set_picked_cone(cone);
  viewport.update();

  // TODO::::::::: generate a cone for the screenspace point using Camera::ray_for_clipspace_point
}
