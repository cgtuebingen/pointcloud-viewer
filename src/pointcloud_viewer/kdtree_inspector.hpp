#ifndef POINTCLOUDVIEWER_KDTREE_INSPECTOR_HPP_
#define POINTCLOUDVIEWER_KDTREE_INSPECTOR_HPP_

#include <QMainWindow>

#include <geometry/aabb.hpp>

class PointCloud;

/**
This class is used to inspect the kd-tree in debug mode
*/
class KdTreeInspector final : public QObject
{
Q_OBJECT
Q_PROPERTY(bool canBuildKdTree READ canBuildKdTree WRITE setCanBuildKdTree NOTIFY canBuildKdTreeChanged)
Q_PROPERTY(bool hasKdTreeAvailable READ hasKdTreeAvailable WRITE setHasKdTreeAvailable NOTIFY hasKdTreeAvailableChanged)
Q_PROPERTY(bool autoBuildKdTreeAfterLoading READ autoBuildKdTreeAfterLoading WRITE setAutoBuildKdTreeAfterLoading NOTIFY autoBuildKdTreeAfterLoadingChanged)
public:
  KdTreeInspector(QWidget* window);
  ~KdTreeInspector();

  bool canBuildKdTree() const;
  bool hasKdTreeAvailable() const;
  bool autoBuildKdTreeAfterLoading() const;

public slots:
  void unload_all_point_clouds();
  void handle_new_point_cloud(QSharedPointer<PointCloud> point_cloud);

  void build_kdtree();

  void kd_tree_inspection_move_to_root();
  void kd_tree_inspection_move_to_parent();
  void kd_tree_inspection_move_to_subtree();
  void kd_tree_inspection_select_left();
  void kd_tree_inspection_select_right();

  void setAutoBuildKdTreeAfterLoading(bool autoBuildKdTreeAfterLoading);

signals:
  void canBuildKdTreeChanged(bool canBuildKdTree);
  void hasKdTreeAvailableChanged(bool hasKdTreeAvailable);
  void kd_tree_inspection_changed(aabb_t active_aabb, glm::vec3 separating_point, aabb_t other_aabb);
  void autoBuildKdTreeAfterLoadingChanged(bool autoBuildKdTreeAfterLoading);

private:
  QWidget* const window;
  QSharedPointer<PointCloud> point_cloud;
  size_t kd_tree_inspection_current_point;
  bool _left_selected = true;

  bool m_canBuildKdTree = false;
  bool m_hasKdTreeAvailable;

  void set_current_point_for_the_kd_tree_inspection(size_t kd_tree_inspection_current_point);
  void update_kd_tree_inspection();

  bool m_autoBuildKdTreeAfterLoading;

private slots:
  void setCanBuildKdTree(bool canBuildKdTree);
  void setHasKdTreeAvailable(bool hasKdTreeAvailable);
};

#endif // POINTCLOUDVIEWER_KDTREE_INSPECTOR_HPP_
