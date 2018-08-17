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
public:
  PointCloudInspector(Viewport* viewport);
  ~PointCloudInspector();

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void pick_point(glm::vec2 screenspace_point);

private:
  QSharedPointer<PointCloud> point_cloud;
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
