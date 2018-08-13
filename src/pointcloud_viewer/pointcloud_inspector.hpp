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
Q_PROPERTY(bool canBuildKdTree READ canBuildKdTree WRITE setCanBuildKdTree NOTIFY canBuildKdTreeChanged)
Q_PROPERTY(bool hasKdTreeAvailable READ hasKdTreeAvailable WRITE setHasKdTreeAvailable NOTIFY hasKdTreeAvailableChanged)
public:
  bool canBuildKdTree() const;
  bool hasKdTreeAvailable() const;

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void build_kdtree();

signals:
  void canBuildKdTreeChanged(bool canBuildKdTree);
  void hasKdTreeAvailableChanged(bool hasKdTreeAvailable);

private:
  QSharedPointer<PointCloud> point_cloud;

  bool m_canBuildKdTree = false;
  bool m_hasKdTreeAvailable;

private slots:
  void setCanBuildKdTree(bool canBuildKdTree);
  void setHasKdTreeAvailable(bool hasKdTreeAvailable);
};

#endif // POINTCLOUDVIEWER_POINTCLOUD_INSPECTOR_HPP_
