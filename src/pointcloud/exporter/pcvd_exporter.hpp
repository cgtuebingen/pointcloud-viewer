#ifndef POINTCLOUD_WORKERS_EXPORTER_PCVD_HPP_
#define POINTCLOUD_WORKERS_EXPORTER_PCVD_HPP_

#include <pointcloud/exporter/abstract_exporter.hpp>

class PcvdExporter final : public AbstractPointCloudExporter
{
public:
  PcvdExporter(const std::string& output_file, const PointCloud& pointcloud);

  bool save_kd_tree = true;
  bool save_vertex_data = false;
  bool save_shader = true;

protected:
  bool export_implementation() override;
};

#endif // POINTCLOUD_WORKERS_EXPORTER_PCVD_HPP_
