#ifndef POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_
#define POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_

#include <pointcloud/importer/abstract_importer.hpp>

/**
Implementation for loading obj files
*/

class AssimpImporter final : public AbstractPointCloudImporter
{
public:
  AssimpImporter(const std::string& input_file, int64_t total_num_bytes);

protected:
  bool import_implementation() override;
};

#endif // POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_
