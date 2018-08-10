#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>

#include <QProgressDialog>
#include <QThread>

using namespace implementation;

void build_kdtree(QWidget* parent, PointCloud* pointCloud)
{
  Q_ASSERT(!pointCloud->can_build_kdtree());

  QProgressDialog progressDialog(QString("Building KD-Tree\n<%1>"), "&Abort", 0, 65536, parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

  QThread thread;
  KdTreeBuilder builder;

  builder.moveToThread(&thread);

  QObject::connect(&thread, &QThread::started, &builder, &KdTreeBuilder::build);
  QObject::connect(&builder, &KdTreeBuilder::finished, &progressDialog, &QProgressDialog::accept);

  thread.start();
  progressDialog.exec();

  thread.wait();
}


namespace implementation {

void KdTreeBuilder::build()
{
  pointCloud->build_kd_tree([this](size_t done, size_t total) -> bool{progress(done, total);return _is_aborted;});

  if(!_is_aborted)
    return finished();
}

void KdTreeBuilder::abort()
{
  _is_aborted = true;
}

} // namespace implementation
