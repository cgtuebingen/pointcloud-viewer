#ifndef POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
#define POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_

#include <QMainWindow>

class PointCloud;

/**
This class is used to inspect the point cloud data
*/
class PointCloudInspector final : public QObject
{
Q_OBJECT
public:

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

private:
  QSharedPointer<PointCloud> point_cloud;
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
