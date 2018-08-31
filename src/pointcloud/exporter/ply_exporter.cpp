#include <pointcloud/exporter/ply_exporter.hpp>
#include <fstream>

typedef data_type::BASE_TYPE BASE_TYPE;

const char* format_data_type(data_type::base_type_t type);

PlyExporter::PlyExporter(const std::string& output_file, const PointCloud& pointcloud)
  : AbstractPointCloudExporter(output_file, pointcloud)
{
}

bool PlyExporter::export_implementation()
{
  std::ofstream stream(output_file, std::ios_base::out); // not a binary stream

  if(!stream.is_open())
    return false;

  const int num_properties = pointcloud.user_data_types.length();

  stream << "ply\n";
  stream << "format ascii 1.0\n";
  stream << "element vertex " << pointcloud.num_points << "\n";
  for(int i=0; i<num_properties; ++i)
    stream << "property " << format_data_type(pointcloud.user_data_types[i]) << " " << pointcloud.user_data_names[i].toStdString() << "\n";
  stream << "end_header\n";

  const uint8_t* data = pointcloud.user_data.data();
  const data_type::base_type_t* data_types = pointcloud.user_data_types.data();
  for(size_t point_index=0; point_index<pointcloud.num_points; ++point_index)
  {
    for(int i=0; i<num_properties; ++i)
    {
      if(i != 0)
        stream << ' ';

      data += read_value_from_buffer_to_stream(stream, data_types[i], data);
    }

    stream << "\n";
  }

  stream << "end_header\n";
  return true;
}

const char* format_data_type(data_type::base_type_t type)
{
  switch(type)
  {
  case BASE_TYPE::INT8:
    return "char";
  case BASE_TYPE::UINT8:
    return "uchar";
  case BASE_TYPE::INT16:
    return "short";
  case BASE_TYPE::UINT16:
    return "ushort";
  case BASE_TYPE::INT32:
  case BASE_TYPE::INT64:
    return "int";
  case BASE_TYPE::UINT32:
  case BASE_TYPE::UINT64:
    return "uint";
  case BASE_TYPE::FLOAT32:
    return "float";
  case BASE_TYPE::FLOAT64:
    return "double";
  }

  Q_UNREACHABLE();
  return "double";
}
