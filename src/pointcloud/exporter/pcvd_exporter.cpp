#include <pointcloud/exporter/pcvd_exporter.hpp>
#include <pointcloud/pcvd_file_format.hpp>
#include <fstream>

PcvdExporter::PcvdExporter(const std::string& output_file, const PointCloud& pointcloud)
  : AbstractPointCloudExporter(output_file, pointcloud)
{
}

bool PcvdExporter::export_implementation()
{
  std::ofstream stream(output_file, std::ios_base::out | std::ios_base::binary); // a binary stream

  pcvd_format::header_t header;

  header.magic_number = pcvd_format::header_t::expected_macic_number();
  header.file_version_number = 0;
  header.downwards_compatibility_version_number = 0;

  header.number_points = pointcloud.num_points;

  header.point_data_stride = decltype(header.point_data_stride)(pointcloud.user_data_stride);
  if(header.point_data_stride != pointcloud.user_data_stride)
  {
    std::cerr << "internal error: more properties than supported";
    return false;
  }

  header.number_fields = decltype(header.number_fields)(pointcloud.user_data_names.length());
  if(header.number_fields != pointcloud.user_data_names.length())
  {
    std::cerr << "internal error: more properties than supported";
    return false;
  }

  QVector<pcvd_format::field_description_t> field_descriptions;
  std::string joined_field_names;
  for(int i=0; i<header.number_fields; ++i)
  {
    const std::string name = pointcloud.user_data_names[i].toStdString();
    pcvd_format::field_description_t field_description;

    joined_field_names += name;
    field_description.name_length = decltype(field_description.name_length)(name.length());
    if(field_description.name_length != name.length())
    {
      std::cerr << "internal error: property name " << name << " too long";
      return false;
    }
    field_description.type = pointcloud.user_data_types[i];
  }

  header.field_names_total_size = decltype(header.point_data_stride)(joined_field_names.length());
  if(header.field_names_total_size != joined_field_names.length())
  {
    std::cerr << "internal error: property names too long";
    return false;
  }

  save_kd_tree = save_kd_tree && pointcloud.has_build_kdtree();

  header.flags = (save_kd_tree ? 0b1 : 0) | (save_vertex_data ? 0b10 : 0);

  header.aabbt = pointcloud.aabb;

  header.reserved = 0;

  std::streamsize header_size = sizeof(pcvd_format::header_t);
  std::streamsize field_headers_size = sizeof(pcvd_format::field_description_t) * header.number_fields;
  std::streamsize field_names_size = std::streamsize(joined_field_names.length());
  std::streamsize vertex_data_size = save_vertex_data ? std::streamsize(pointcloud.num_points * sizeof(PointCloud::vertex_t)) : 0;
  std::streamsize point_data_size = std::streamsize(pointcloud.num_points * header.point_data_stride);
  std::streamsize kd_tree_size = save_kd_tree ? std::streamsize(pointcloud.num_points * sizeof(size_t)) : 0;
  total_progress = header_size + field_headers_size + field_names_size + vertex_data_size + point_data_size + kd_tree_size;
  int64_t current_progress = 0;

  stream.write(reinterpret_cast<const char*>(&header), header_size);
  if(!handle_written_chunk(current_progress += header_size))
    return false;

  stream.write(reinterpret_cast<const char*>(field_descriptions.data()), field_headers_size);
  if(!handle_written_chunk(current_progress += field_headers_size))
    return false;

  stream.write(joined_field_names.c_str(), field_names_size);
  if(!handle_written_chunk(current_progress += field_names_size))
    return false;

  if(save_vertex_data)
  {
    stream.write(reinterpret_cast<const char*>(pointcloud.coordinate_color.data()), vertex_data_size);
    if(!handle_written_chunk(current_progress += vertex_data_size))
      return false;
  }
  stream.write(reinterpret_cast<const char*>(pointcloud.user_data.data()), point_data_size);
  if(!handle_written_chunk(current_progress += point_data_size))
    return false;
  if(save_kd_tree)
  {
    stream.write(reinterpret_cast<const char*>(pointcloud.kdtree_index.data()), kd_tree_size);
    if(!handle_written_chunk(current_progress += kd_tree_size))
      return false;
  }

  return true;
}
