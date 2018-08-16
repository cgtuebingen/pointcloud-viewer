#include <pointcloud_viewer/workers/kdtree_builder_dialog.hpp>
#include <core_library/print.hpp>

#include <QProgressDialog>
#include <QCoreApplication>
#include <QThread>

using namespace implementation;

void build_kdtree(QWidget* parent, PointCloud* pointCloud)
{
  Q_ASSERT(pointCloud->can_build_kdtree());

  QThread thread;
  thread.setObjectName("build_kdtree");
  KdTreeBuilder builder(*pointCloud);

  builder.moveToThread(&thread);

  QProgressDialog progressDialog(QString("Building KD-Tree"), "&Abort", 0, int(builder.max_progress), parent);
  progressDialog.setWindowModality(Qt::ApplicationModal);

  QObject::connect(&progressDialog, &QProgressDialog::canceled, &builder, &KdTreeBuilder::abort, Qt::DirectConnection);
  QObject::connect(&thread, &QThread::started, &builder, &KdTreeBuilder::build);
  QObject::connect(&builder, &KdTreeBuilder::finished, &thread, &QThread::quit, Qt::QueuedConnection);
  QObject::connect(&builder, &KdTreeBuilder::finished, &progressDialog, &QProgressDialog::accept, Qt::QueuedConnection);
  QObject::connect(&builder, &KdTreeBuilder::progress, &progressDialog, &QProgressDialog::setValue);

  thread.start();
  progressDialog.exec();

  while(!thread.wait(10))
    QCoreApplication::processEvents(QEventLoop::EventLoopExec | QEventLoop::DialogExec | QEventLoop::WaitForMoreEvents);
}


namespace implementation {

KdTreeBuilder::KdTreeBuilder(PointCloud& pointCloud)
  : pointCloud(pointCloud)
{
}

void KdTreeBuilder::build()
{
  pointCloud.build_kd_tree([this](size_t done, size_t total) -> bool{
    size_t progress = (done*max_progress)/total;
//    println("done: ", done, "  total: ", total, "  progress", progress);
    this->progress(int(progress));
    return !_is_aborted;
  });

  return finished();
}

void KdTreeBuilder::abort()
{
  _is_aborted = true;
}

} // namespace implementation
