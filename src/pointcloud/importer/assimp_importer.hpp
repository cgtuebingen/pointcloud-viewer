#ifndef POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_
#define POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_

#ifdef USE_ASSIMP

#include <pointcloud/importer/abstract_importer.hpp>

/**
Implementation for loading obj files
*/

class AssimpImporter final : public AbstractPointCloudImporter
{
public:
  AssimpImporter(const std::string& input_file);

protected:
  bool import_implementation() override;
};

#endif // USE_ASSIMP

#endif // POINTCLOUD_WORKERS_IMPORTER_ASSIMP_HPP_
