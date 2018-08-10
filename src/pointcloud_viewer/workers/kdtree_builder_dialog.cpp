#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>

#include <QProgressDialog>

void build_kdtree(QWidget* parent, PointCloud* pointCloud)
{
  QProgressDialog progressDialog(QString("Building KD-Tree\n<%1>"), "&Abort", 0, pointCloud->num_points, parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

}
