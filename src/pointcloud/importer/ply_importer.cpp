#include <pointcloud/importer/ply_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <QThread>
#include <QAbstractEventDispatcher>

#include <iostream>

PlyImporter::PlyImporter(std::istream& input_stream, int64_t total_num_bytes)
  : AbstractPointCloudImporter(input_stream, total_num_bytes)
{
}

bool PlyImporter::import_implementation()
{
#if 0
  point_cloud.set_data(PointCloud::COLUMN::COORDINATES, get_type(vertices, 3), vertices->buffer.get(), vertices->buffer.size_bytes());
    point_cloud.set_data(PointCloud::COLUMN::COLOR, get_type(colors, 3), colors->buffer.get(), colors->buffer.size_bytes());
    point_cloud.set_data(PointCloud::COLUMN::USER_DATA, get_type(user_data, int(data_components.size())), user_data->buffer.get(), user_data->buffer.size_bytes());
#endif
  return true;
}
