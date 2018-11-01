#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>
#include <pointcloud/pointcloud.hpp>
#include <core_library/types.hpp>
#include <core_library/print.hpp>
#include <glm/gtx/io.hpp>

#include <QSettings>
#include <QMessageBox>

PointCloudInspector::PointCloudInspector(Viewport* viewport)
  : viewport(*viewport)
{
  QSettings settings;
  m_pointSelectionHighlightRadius = settings.value("UI/SelectionHighlightRadius", 0.5).toDouble();
}

PointCloudInspector::~PointCloudInspector()
{
  QSettings settings;
  settings.setValue("UI/SelectionHighlightRadius", m_pointSelectionHighlightRadius);
}

double PointCloudInspector::pointSelectionHighlightRadius() const
{
  return m_pointSelectionHighlightRadius;
}

bool PointCloudInspector::hasSelectedPoint() const
{
  return _selected_point != KDTreeIndex::point_index_t::INVALID;
}

int PointCloudInspector::pickRadius() const
{
  return m_pickRadius;
}

// Called when athe point-cloud was unloaded
void PointCloudInspector::unload_all_point_clouds()
{
  setSelectedPoint(KDTreeIndex::point_index_t::INVALID);

  this->point_cloud.clear();
}

// Called when a point-cloud was loaded
void PointCloudInspector::handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud)
{
  this->point_cloud = point_cloud;
}

void PointCloudInspector::pick_point(glm::ivec2 pixel)
{
  if(!point_cloud)
    return;

  if(!point_cloud->has_build_kdtree())
  {
    QMessageBox msg_box(QMessageBox::Information,
                        "No KD-Tree built",
                        "In order to be able to pick points, a KD-Tree must have been built.\n\nBuild the KD-Tree now?",
                        QMessageBox::Yes | QMessageBox::No,
                        &viewport);
    msg_box.setModal(true);

    if(msg_box.exec() == QMessageBox::Yes)
      ::build_kdtree(&viewport, this->point_cloud.data());

    if(!point_cloud->has_build_kdtree())
      return;
  }

  float pick_radius = glm::max(4.f, glm::ceil(m_pickRadius + 2.f));
  glm::ivec2 viewport_size(viewport.width(), viewport.height());

  glm::vec2 screenspace_point = Camera::pixel_to_screenspace(pixel, viewport_size);

  ray_t center_ray = viewport.navigation.camera.ray_for_screenspace_point(screenspace_point);
  ray_t side_ray = viewport.navigation.camera.ray_for_screenspace_point(Camera::pixel_to_screenspace(pixel+glm::ivec2(pick_radius, 0), viewport_size));

  float angle = glm::acos(glm::dot(center_ray.direction, side_ray.direction));

  cone_t cone = cone_t::cone_from_ray_angle(center_ray, angle);

  viewport.visualization().set_picked_cone(cone);

  KDTreeIndex::point_index_t point = point_cloud->kdtree_index.pick_point(cone, point_cloud->coordinate_color.data(), PointCloud::stride);

  setSelectedPoint(point);
}

void PointCloudInspector::update()
{
  if(hasSelectedPoint())
  {
    PointCloud::vertex_t vertex = get_selected_point();
    viewport.visualization().select_picked_point(vertex.coordinate, vertex.color);
    viewport.navigation.setSelectedPoint(vertex.coordinate);
    this->selected_point(vertex.coordinate,
                         vertex.color,
                         point_cloud->all_values_of_point(size_t(_selected_point)));
  }else
  {
    viewport.visualization().deselect_picked_point();
    viewport.navigation.unsetSelectedPoint();
    this->deselect_picked_point();
  }
  viewport.update();
}

void PointCloudInspector::setPointSelectionHighlightRadius(double pointSelectionHighlightRadius)
{
  qWarning("Floating point comparison needs context sanity check");
  if (qFuzzyCompare(m_pointSelectionHighlightRadius, pointSelectionHighlightRadius))
    return;

  m_pointSelectionHighlightRadius = pointSelectionHighlightRadius;
  emit pointSelectionHighlightRadiusChanged(m_pointSelectionHighlightRadius);
}

void PointCloudInspector::setSelectedPoint(KDTreeIndex::point_index_t selected_point)
{
  if (_selected_point == selected_point)
    return;

  _selected_point = selected_point;

  emit hasSelectedPointChanged(hasSelectedPoint());
  this->update();
}

void PointCloudInspector::setPickRadius(int pickRadius)
{
  if (m_pickRadius == pickRadius)
    return;

  m_pickRadius = pickRadius;
  emit pickRadiusChanged(m_pickRadius);
}

PointCloud::vertex_t PointCloudInspector::get_selected_point(PointCloud::vertex_t fallback) const
{
  if(hasSelectedPoint())
    return point_cloud->vertex(size_t(_selected_point));
  else
    return fallback;
}
