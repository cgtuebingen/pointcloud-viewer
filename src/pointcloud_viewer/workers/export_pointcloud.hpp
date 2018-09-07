#ifndef POINTCLOUDVIEWER_WORKERS_EXPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_EXPORTPOINTCLOUD_HPP_

#include <pointcloud/pointcloud.hpp>
#include <QObject>

/**
The function responsible for export point clouds.
*/
bool export_point_cloud(QWidget* parent, QString file, const PointCloud& pointcloud, QString selectedFilter);

#endif // POINTCLOUDVIEWER_WORKERS_EXPORTPOINTCLOUD_HPP_
