#ifndef POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
#define POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_

#include <QProgressDialog>
#include <functional>

bool import_point_cloud(QWidget* parent, QString file);

bool import_ply_file(std::istream& input_stream, std::function<bool(int64_t processed_bytes)> callback);

#endif // POINTCLOUDVIEWER_WORKERS_IMPORTPOINTCLOUD_HPP_
