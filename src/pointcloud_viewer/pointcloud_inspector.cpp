#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>
#include <pointcloud/pointcloud.hpp>

#include <QDebug>

bool PointCloudInspector::canBuildKdTree() const
{
  return m_canBuildKdTree;
}

bool PointCloudInspector::hasKdTreeAvailable() const
{
  return m_hasKdTreeAvailable;
}

void PointCloudInspector::unload_all_point_clouds()
{
  this->point_cloud.clear();

  setCanBuildKdTree(false);
  setHasKdTreeAvailable(false);
}

void PointCloudInspector::handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;

  this->setCanBuildKdTree(this->point_cloud->can_build_kdtree());
  this->setHasKdTreeAvailable(this->point_cloud->has_build_kdtree());
}

void PointCloudInspector::build_kdtree()
{
  Q_ASSERT(this->point_cloud != nullptr);
  Q_ASSERT(this->point_cloud->can_build_kdtree());

  this->setCanBuildKdTree(false);

  ::build_kdtree(nullptr, this->point_cloud.data());

  this->setCanBuildKdTree(this->point_cloud->can_build_kdtree());
  this->setHasKdTreeAvailable(this->point_cloud->has_build_kdtree());

  if(this->point_cloud->has_build_kdtree())
    kd_tree_inspection_changed(this->point_cloud->aabb, glm::vec3(INFINITY), aabb_t::invalid());
}

void PointCloudInspector::setCanBuildKdTree(bool canBuildKdTree)
{
  if (m_canBuildKdTree == canBuildKdTree)
    return;

  m_canBuildKdTree = canBuildKdTree;
  emit canBuildKdTreeChanged(m_canBuildKdTree);
}

void PointCloudInspector::setHasKdTreeAvailable(bool hasKdTreeAvailable)
{
  if (m_hasKdTreeAvailable == hasKdTreeAvailable)
    return;

  m_hasKdTreeAvailable = hasKdTreeAvailable;

  if(!hasKdTreeAvailable)
    kd_tree_inspection_changed(aabb_t::invalid(), glm::vec3(INFINITY), aabb_t::invalid());

  emit hasKdTreeAvailableChanged(m_hasKdTreeAvailable);
}
