#ifndef POINTCLOUDVIEWER_WORKERS_KDTREE_BUILDER_DIALOG_H
#define POINTCLOUDVIEWER_WORKERS_KDTREE_BUILDER_DIALOG_H

#include <pointcloud/pointcloud.hpp>
#include <QObject>

void build_kdtree(QWidget* parent, PointCloud* pointCloud);

namespace implementation {

class KdTreeBuilder : public QObject
{
  Q_OBJECT
public:
  PointCloud& pointCloud;
  const size_t max_progress = 65535;

  KdTreeBuilder(PointCloud& pointCloud);

public slots:
  void build();
  void abort();

signals:
  void progress(int);
  void finished();

private:
 volatile bool _is_aborted = false;
};

} // implementation

#endif // KDTREE_BUILDER_DIALOG_H
