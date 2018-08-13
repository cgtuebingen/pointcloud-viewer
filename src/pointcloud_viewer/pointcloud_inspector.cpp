#include <pointcloud_viewer/pointcloud_inspector.hpp>

#include <QDebug>

void PointCloudInspector::unload_all_point_clouds()
{
  this->point_cloud.clear();
}

void PointCloudInspector::handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;
}
