#include <pointcloud/importer/assimp_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <QThread>
#include <QAbstractEventDispatcher>

#include <iostream>

// see http://assimp.sourceforge.net/lib_html/usage.html

AssimpImporter::AssimpImporter(const std::string& input_file, int64_t total_num_bytes)
  : AbstractPointCloudImporter(input_file, total_num_bytes)
{
}

bool AssimpImporter::import_implementation()
{
  Assimp::Importer importer;

  importer.ReadFile(input_file,
                    aiProcess_PreTransformVertices |
                    aiProcess_ImproveCacheLocality);

  TODO

#if 0
  point_cloud.set_data(PointCloud::COLUMN::COORDINATES, get_type(vertices, 3), vertices->buffer.get(), vertices->buffer.size_bytes());
    point_cloud.set_data(PointCloud::COLUMN::COLOR, get_type(colors, 3), colors->buffer.get(), colors->buffer.size_bytes());
    point_cloud.set_data(PointCloud::COLUMN::USER_DATA, get_type(user_data, int(data_components.size())), user_data->buffer.get(), user_data->buffer.size_bytes());
#endif
  return true;
}
