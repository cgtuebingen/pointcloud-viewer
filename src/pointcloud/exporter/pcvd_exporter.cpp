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
  header.file_version_number = 1;
  header.downwards_compatibility_version_number = 0;

  header.number_points = pointcloud.num_points;

  header.point_data_stride = decltype(header.point_data_stride)(pointcloud.user_data_stride);
  if(header.point_data_stride != pointcloud.user_data_stride)
    throw QString("More properties than supported by the file format (point stride too large)");

  header.number_fields = decltype(header.number_fields)(pointcloud.user_data_names.length());
  if(header.number_fields != pointcloud.user_data_names.length())
    throw QString("More properties than supported by the file format");

  QVector<pcvd_format::field_description_t> field_descriptions;
  std::string joined_field_names;
  for(int i=0; i<header.number_fields; ++i)
  {
    const std::string name = pointcloud.user_data_names[i].toStdString();
    pcvd_format::field_description_t field_description;

    joined_field_names += name;
    field_description.name_length = decltype(field_description.name_length)(name.length());
    if(field_description.name_length != name.length())
      throw QString("More properties than supported by the file format (property name too long)");
    field_description.type = pointcloud.user_data_types[i];

    field_descriptions << field_description;
  }

  header.field_names_total_size = decltype(header.point_data_stride)(joined_field_names.length());
  if(header.field_names_total_size != joined_field_names.length())
    throw QString("More properties than supported by the file format (property names too long)");

  save_kd_tree = save_kd_tree && pointcloud.has_build_kdtree();

  header.flags = (save_kd_tree ? 0b1 : 0) | (save_vertex_data ? 0b10 : 0) | (save_shader ? 0b100 : 0);

  header.aabb = pointcloud.aabb;

  header.reserved = 0;

  std::streamsize header_size = sizeof(pcvd_format::header_t);
  std::streamsize field_headers_size = sizeof(pcvd_format::field_description_t) * header.number_fields;
  std::streamsize field_names_size = header.field_names_total_size;
  std::streamsize vertex_data_size = save_vertex_data ? std::streamsize(pointcloud.num_points * sizeof(PointCloud::vertex_t)) : 0;
  std::streamsize point_data_size = std::streamsize(pointcloud.num_points * header.point_data_stride);
  std::streamsize kd_tree_size = save_kd_tree ? std::streamsize(pointcloud.num_points * sizeof(size_t)) : 0;
  total_progress = header_size + field_headers_size + field_names_size + vertex_data_size + point_data_size + kd_tree_size;
  int64_t current_progress = 0;

  stream.write(reinterpret_cast<const char*>(&header), header_size);
  handle_written_chunk(current_progress += header_size);

  stream.write(reinterpret_cast<const char*>(field_descriptions.data()), field_headers_size);
  handle_written_chunk(current_progress += field_headers_size);

  stream.write(joined_field_names.c_str(), field_names_size);
  handle_written_chunk(current_progress += field_names_size);

  if(save_vertex_data)
  {
    stream.write(reinterpret_cast<const char*>(pointcloud.coordinate_color.data()), vertex_data_size);
    handle_written_chunk(current_progress += vertex_data_size);
  }
  stream.write(reinterpret_cast<const char*>(pointcloud.user_data.data()), point_data_size);
  handle_written_chunk(current_progress += point_data_size);
  if(save_kd_tree)
  {
    stream.write(reinterpret_cast<const char*>(pointcloud.kdtree_index.data()), kd_tree_size);
    handle_written_chunk(current_progress += kd_tree_size);
  }

  {
    QByteArray name_bytes = pointcloud.shader.name.toUtf8();
    QByteArray coordinate_bytes = pointcloud.shader.coordinate_expression.toUtf8();
    QByteArray color_bytes = pointcloud.shader.color_expression.toUtf8();
    QByteArray node_bytes = pointcloud.shader.node_data.toUtf8();
    pcvd_format::shader_description_t shader_description;

    if(name_bytes.length() > std::numeric_limits<decltype(shader_description.name_length)>::max())
      throw QString("Can't save point cloud (shader name too long)");
    if(coordinate_bytes.length() > std::numeric_limits<decltype(shader_description.coordinate_expression_length)>::max())
      throw QString("Can't save point cloud (shader coordinate expression too long)");
    if(color_bytes.length() > std::numeric_limits<decltype(shader_description.color_expression_length)>::max())
      throw QString("Can't save point cloud (shader color expression too long)");
    if(node_bytes.length() > std::numeric_limits<decltype(shader_description.node_data_length)>::max())
      throw QString("Can't save point cloud (shader node data string too long)");

    shader_description.name_length = static_cast<decltype(shader_description.name_length)>(name_bytes.length());
    shader_description.coordinate_expression_length = static_cast<decltype(shader_description.coordinate_expression_length)>(coordinate_bytes.length());
    shader_description.color_expression_length = static_cast<decltype(shader_description.color_expression_length)>(color_bytes.length());
    shader_description.node_data_length = static_cast<decltype(shader_description.node_data_length)>(node_bytes.length());
    stream.write(reinterpret_cast<const char*>(&shader_description), sizeof(shader_description));
    stream.write(name_bytes.data(), name_bytes.length());
    stream.write(coordinate_bytes.data(), name_bytes.length());
    stream.write(color_bytes.data(), name_bytes.length());
    stream.write(node_bytes.data(), name_bytes.length());
  }

  return true;
}
