#ifndef POINTCLOUD_WORKERS_IMPORTER_PLY_HPP_
#define POINTCLOUD_WORKERS_IMPORTER_PLY_HPP_

#include <pointcloud/importer/abstract_importer.hpp>

/**
Implementation for loading ply files
*/

class PlyImporter final : public AbstractPointCloudImporter
{
public:
  PlyImporter(const std::string& input_file, int64_t total_num_bytes);

protected:
  bool import_implementation() override;
};

#endif // POINTCLOUD_WORKERS_IMPORTER_PLY_HPP_
