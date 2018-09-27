#include <pointcloud_viewer/pointcloud_inspector.hpp>
#include <pointcloud_viewer/viewport.hpp>
#include <pointcloud_viewer/visualizations.hpp>
#include <pointcloud/pointcloud.hpp>
#include <core_library/types.hpp>
#include <core_library/print.hpp>
#include <glm/gtx/io.hpp>

#include <QSettings>

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
  return m_hasSelectedPoint;
}

int PointCloudInspector::pickRadius() const
{
  return m_pickRadius;
}

// Called when athe point-cloud was unloaded
void PointCloudInspector::unload_all_point_clouds()
{
  viewport.visualization().deselect_picked_point();
  deselect_picked_point();
  setHasSelectedPoint(false);

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

  float pick_radius = glm::max(4.f, glm::ceil(m_pickRadius + 2.f));
  glm::ivec2 viewport_size(viewport.width(), viewport.height());

  glm::vec2 screenspace_point = Camera::pixel_to_screenspace(pixel, viewport_size);

  ray_t center_ray = viewport.navigation.camera.ray_for_screenspace_point(screenspace_point);
  ray_t side_ray = viewport.navigation.camera.ray_for_screenspace_point(Camera::pixel_to_screenspace(pixel+glm::ivec2(pick_radius, 0), viewport_size));

  float angle = glm::acos(glm::dot(center_ray.direction, side_ray.direction));

  cone_t cone = cone_t::cone_from_ray_angle(center_ray, angle);

  viewport.visualization().set_picked_cone(cone);

  KDTreeIndex::point_index_t point = point_cloud->kdtree_index.pick_point(cone, point_cloud->coordinate_color.data(), PointCloud::stride);

  if(point == KDTreeIndex::point_index_t::INVALID)
  {
    viewport.visualization().deselect_picked_point();
    deselect_picked_point();
    setHasSelectedPoint(false);
  }else
  {
    size_t index = size_t(point);

    glm::vec3 coordinate = read_value_from_buffer<glm::vec3>(point_cloud->coordinate_color.data() + index * PointCloud::stride);
    glm::u8vec3 color = read_value_from_buffer<glm::u8vec3>(point_cloud->coordinate_color.data() + index * PointCloud::stride + sizeof(glm::vec3));

    viewport.visualization().select_picked_point(coordinate, color);
    selected_point(coordinate, color, point_cloud->all_values_of_point(index));
    setHasSelectedPoint(true);
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

void PointCloudInspector::setHasSelectedPoint(bool hasSelectedPoint)
{
  if (m_hasSelectedPoint == hasSelectedPoint)
    return;

  m_hasSelectedPoint = hasSelectedPoint;
  emit hasSelectedPointChanged(m_hasSelectedPoint);
}

void PointCloudInspector::setPickRadius(int pickRadius)
{
  if (m_pickRadius == pickRadius)
    return;

  m_pickRadius = pickRadius;
  emit pickRadiusChanged(m_pickRadius);
}
