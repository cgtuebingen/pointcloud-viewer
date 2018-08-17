#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud/pointcloud.hpp>
#include <core_library/print.hpp>
#include <glm/gtx/io.hpp>

PointCloudInspector::PointCloudInspector()
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

void PointCloudInspector::pick_point(glm::vec2 point)
{
  point = point * glm::vec2(2.f, -2.f) + glm::vec2(-1.f, +1.f);

  println(point);
}
