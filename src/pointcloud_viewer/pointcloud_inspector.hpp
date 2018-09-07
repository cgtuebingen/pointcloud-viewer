#ifndef POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
#define POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_

#include <QMainWindow>

#include <pointcloud/pointcloud.hpp>
#include <geometry/aabb.hpp>
#include <geometry/ray.hpp>

class Viewport;

/**
This class is used to inspect the data of the pointcloud
*/
class PointCloudInspector final : public QObject
{
Q_OBJECT
Q_PROPERTY(double pointSelectionHighlightRadius READ pointSelectionHighlightRadius WRITE setPointSelectionHighlightRadius NOTIFY pointSelectionHighlightRadiusChanged)
Q_PROPERTY(bool hasSelectedPoint READ hasSelectedPoint WRITE setHasSelectedPoint NOTIFY hasSelectedPointChanged)
Q_PROPERTY(int pickRadius READ pickRadius WRITE setPickRadius NOTIFY pickRadiusChanged)
public:
  PointCloudInspector(Viewport* viewport);
  ~PointCloudInspector();

  double pointSelectionHighlightRadius() const;
  bool hasSelectedPoint() const;
  int pickRadius() const;

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void pick_point(glm::ivec2 pixel);

  void setPointSelectionHighlightRadius(double pointSelectionHighlightRadius);
  void setHasSelectedPoint(bool hasSelectedPoint);
  void setPickRadius(int pickRadius);

signals:
  void deselect_picked_point();
  void selected_point(glm::vec3 coordinate, glm::u8vec3 color, PointCloud::UserData user_data);

  void pointSelectionHighlightRadiusChanged(double pointSelectionHighlightRadius);
  void hasSelectedPointChanged(bool hasSelectedPoint);
  void pickRadiusChanged(int pickRadius);

private:
  Viewport& viewport;
  QSharedPointer<PointCloud> point_cloud;
  double m_pointSelectionHighlightRadius;
  bool m_hasSelectedPoint = false;
  int m_pickRadius = 2;
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
