#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud/pointcloud.hpp>
#include <core_library/print.hpp>
#include <glm/gtx/io.hpp>

PointCloudInspector::PointCloudInspector(Viewport* viewport)
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

void PointCloudInspector::pick_point(glm::vec2 screenspace_point)
{
  if(!point_cloud || !point_cloud->has_build_kdtree())
    return;

  // TODO::::::::: generate a cone for the screenspace point using Camera::ray_for_clipspace_point
}
