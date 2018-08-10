#ifndef POINTCLOUDVIEWER_WORKERS_KDTREE_BUILDER_DIALOG_H
#define POINTCLOUDVIEWER_WORKERS_KDTREE_BUILDER_DIALOG_H

#include <pointcloud/pointcloud.hpp>
#include <QObject>

void build_kdtree(QWidget* parent, PointCloud* pointCloud);

#endif // KDTREE_BUILDER_DIALOG_H
