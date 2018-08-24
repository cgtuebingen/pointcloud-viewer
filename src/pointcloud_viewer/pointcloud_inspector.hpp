#ifndef POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
#define POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_

#include <QMainWindow>

#include <geometry/aabb.hpp>
#include <geometry/ray.hpp>

class PointCloud;
class Viewport;

/**
This class is used to inspect the data of the pointcloud
*/
class PointCloudInspector final : public QObject
{
Q_OBJECT
Q_PROPERTY(double pointSelectionHighlightRadius READ pointSelectionHighlightRadius WRITE setPointSelectionHighlightRadius NOTIFY pointSelectionHighlightRadiusChanged)
Q_PROPERTY(bool hasSelectedPoint READ hasSelectedPoint WRITE setHasSelectedPoint NOTIFY hasSelectedPointChanged)
public:
  PointCloudInspector(Viewport* viewport);
  ~PointCloudInspector();

  double pointSelectionHighlightRadius() const;
  bool hasSelectedPoint() const;

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void pick_point(glm::ivec2 pixel);

  void setPointSelectionHighlightRadius(double pointSelectionHighlightRadius);
  void setHasSelectedPoint(bool hasSelectedPoint);

signals:
  void deselect_picked_point();
  void selected_point(glm::vec3 coordinate, glm::u8vec3 color);

  void pointSelectionHighlightRadiusChanged(double pointSelectionHighlightRadius);
  void hasSelectedPointChanged(bool hasSelectedPoint);

private:
  Viewport& viewport;
  QSharedPointer<PointCloud> point_cloud;
  double m_pointSelectionHighlightRadius;
  bool m_hasSelectedPoint = false;
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
