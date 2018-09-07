#ifndef POINTCLOUD_WORKERS_IMPORTER_PCVD_HPP_
#define POINTCLOUD_WORKERS_IMPORTER_PCVD_HPP_

#include <pointcloud/importer/abstract_importer.hpp>
#include <pointcloud/buffer.hpp>

#include <QVector>

/**
Implementation for loading ply files
*/
class PcvdImporter final : public AbstractPointCloudImporter
{
public:
  PcvdImporter(const std::string& input_file);

protected:
  bool import_implementation() override;

private:
  std::streamsize current_progress = 0;
};

#endif // POINTCLOUD_WORKERS_IMPORTER_PCVD_HPP_
