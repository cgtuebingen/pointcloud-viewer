#include <pointcloud_viewer/kdtree_inspector.hpp>
#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>
#include <pointcloud/pointcloud.hpp>

#include <QDebug>

bool KdTreeInspector::canBuildKdTree() const
{
  return m_canBuildKdTree;
}

bool KdTreeInspector::hasKdTreeAvailable() const
{
  return m_hasKdTreeAvailable;
}

// Called when athe point-cloud was unloaded
void KdTreeInspector::unload_all_point_clouds()
{
  this->point_cloud.clear();

  setCanBuildKdTree(false);
  setHasKdTreeAvailable(false);
  kd_tree_inspection_move_to_root();
}

// Called when a point-cloud was loaded
void KdTreeInspector::handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;

  this->setCanBuildKdTree(this->point_cloud->can_build_kdtree());
  this->setHasKdTreeAvailable(this->point_cloud->has_build_kdtree());
  kd_tree_inspection_move_to_root();
}

// build the kd tree (also showing a progress dialog)
void KdTreeInspector::build_kdtree()
{
  Q_ASSERT(this->point_cloud != nullptr);
  Q_ASSERT(this->point_cloud->can_build_kdtree());

  this->setCanBuildKdTree(false);

  ::build_kdtree(nullptr, this->point_cloud.data());

  this->setCanBuildKdTree(this->point_cloud->can_build_kdtree());
  this->setHasKdTreeAvailable(this->point_cloud->has_build_kdtree());

  kd_tree_inspection_move_to_root();
}

// The kd tree inspection is reset to point to the root
void KdTreeInspector::kd_tree_inspection_move_to_root()
{
  if(this->point_cloud==nullptr || !this->point_cloud->has_build_kdtree())
  {
    kd_tree_inspection_changed(point_cloud ? point_cloud->aabb : aabb_t::invalid(), glm::vec3(INFINITY), aabb_t::invalid());
    return;
  }

  set_current_point_for_the_kd_tree_inspection(point_cloud->kdtree_index.whole_tree().median());
}

void KdTreeInspector::setCanBuildKdTree(bool canBuildKdTree)
{
  if (m_canBuildKdTree == canBuildKdTree)
    return;

  m_canBuildKdTree = canBuildKdTree;
  emit canBuildKdTreeChanged(m_canBuildKdTree);
}

void KdTreeInspector::setHasKdTreeAvailable(bool hasKdTreeAvailable)
{
  if (m_hasKdTreeAvailable == hasKdTreeAvailable)
    return;

  m_hasKdTreeAvailable = hasKdTreeAvailable;

  if(!hasKdTreeAvailable)
    kd_tree_inspection_move_to_root();

  emit hasKdTreeAvailableChanged(m_hasKdTreeAvailable);
}

void KdTreeInspector::set_current_point_for_the_kd_tree_inspection(size_t kd_tree_inspection_current_point)
{
  if(this->point_cloud==nullptr || !this->point_cloud->has_build_kdtree())
    return;

  this->kd_tree_inspection_current_point = kd_tree_inspection_current_point;
  kd_tree_inspection_changed(this->point_cloud->aabb, glm::vec3(INFINITY), aabb_t::invalid());
}
