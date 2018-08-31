#include <pointcloud/exporter/pcvd_exporter.hpp>

PcvdExporter::PcvdExporter(const std::string& output_file, const PointCloud& pointcloud)
  : AbstractPointCloudExporter(output_file, pointcloud)
{
}

bool PcvdExporter::export_implementation()
{
  // TODO
  return false;
}
