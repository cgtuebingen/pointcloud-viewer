#ifndef POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_

#include <pointcloud/pointcloud.hpp>
#include <QObject>

/**
The function responsible for import point clouds.
*/
PointCloud import_point_cloud(QWidget* parent, QString file);

#endif // POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
