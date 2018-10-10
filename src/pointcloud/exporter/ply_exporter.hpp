#ifndef POINTCLOUD_WORKERS_EXPORTER_PLY_HPP_
#define POINTCLOUD_WORKERS_EXPORTER_PLY_HPP_

#include <pointcloud/exporter/abstract_exporter.hpp>

class PlyExporter final : public AbstractPointCloudExporter
{
public:
  PlyExporter(const std::string& output_file, const PointCloud& pointcloud);

protected:
  bool export_implementation() override;
};

#endif // POINTCLOUD_WORKERS_EXPORTER_PLY_HPP_
