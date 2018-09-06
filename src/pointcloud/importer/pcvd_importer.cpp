#include <pointcloud/importer/pcvd_importer.hpp>
#include <pointcloud/pcvd_file_format.hpp>
#include <fstream>

PcvdImporter::PcvdImporter(const std::string& input_file)
  : AbstractPointCloudImporter(input_file)
{
}

bool PcvdImporter::import_implementation()
{
  std::streamsize read_bytes;
  std::ifstream stream(input_file, std::ios_base::in | std::ios_base::binary); // a binary stream

  pcvd_format::header_t header;
  read_bytes = stream.readsome(reinterpret_cast<char*>(&header), sizeof(pcvd_format::header_t));
  if(read_bytes == 0)
    throw QString("Can't load empty file");

  if(read_bytes < 4)
    throw QString("Can't load corrupt file");

  if(header.magic_number != pcvd_format::header_t::expected_macic_number())
    throw QString("Wrong file format");

  if(read_bytes != sizeof(pcvd_format::header_t))
    throw QString("Can't load corrupt file");

  if(header.downwards_compatibility_version_number > 0)
    throw QString("Incompatible file format version");

  if(header.number_points == 0)
    throw QString("Need at least one point");
  if(header.number_fields == 0)
    throw QString("Need at least one field");
  if(header.point_data_stride == 0)
    throw QString("corrupt header (point_data_stride)");
  if(header.field_names_total_size == 0)
    throw QString("corrupt header (field_names_total_size)");

  if(header.file_version_number == 0 && (header.flags&0xfffc)!=0)
    throw QString("corrupt header (invalid flags)");
  if(header.file_version_number == 0 && header.reserved!=0)
    throw QString("corrupt header (invalid padding)");
  if(glm::any(glm::isnan(header.aabb.min_point)))
    throw QString("corrupt header (invalid aabb)");
  if(glm::any(glm::isnan(header.aabb.max_point)))
    throw QString("corrupt header (invalid aabb)");
  if(glm::any(glm::isinf(header.aabb.min_point)))
    throw QString("corrupt header (invalid aabb)");
  if(glm::any(glm::isinf(header.aabb.max_point)))
    throw QString("corrupt header (invalid aabb)");

  const bool load_kd_tree = header.flags & 0b1;
  const bool load_vertex = header.flags & 0b10;

  std::streamsize header_size = sizeof(pcvd_format::header_t);
  std::streamsize field_headers_size = sizeof(pcvd_format::field_description_t) * header.number_fields;
  std::streamsize field_names_size = header.field_names_total_size;
  std::streamsize vertex_data_size = std::streamsize(pointcloud.num_points * sizeof(PointCloud::vertex_t));
  std::streamsize point_data_size = std::streamsize(pointcloud.num_points * header.point_data_stride);
  std::streamsize kd_tree_size = load_kd_tree ? std::streamsize(pointcloud.num_points * sizeof(size_t)) : 0;
  total_progress = header_size + field_headers_size + field_names_size + vertex_data_size + point_data_size + kd_tree_size;

  handle_loaded_chunk(current_progress += header_size);

  QVector<pcvd_format::field_description_t> field_descriptions;
  std::string joined_field_names;
  field_descriptions.resize(header.number_fields);
  joined_field_names.resize(header.field_names_total_size);
  read_bytes = stream.readsome(reinterpret_cast<char*>(field_descriptions.data()), field_headers_size);
  if(read_bytes != field_headers_size)
    throw QString("Incomplete file!");
  read_bytes = stream.readsome(reinterpret_cast<char*>(field_descriptions.data()), field_names_size);
  if(read_bytes != field_names_size)
    throw QString("Incomplete file!");

  QVector<QString> field_names;
  QVector<data_type::base_type_t> field_types;
  QVector<size_t> field_data_offset;
  field_names.reserve(header.number_fields);
  field_types.reserve(header.number_fields);
  field_data_offset.reserve(header.number_fields);
  uint16_t fields_total_stride = 0;
  uint16_t fields_total_name_length = 0;
  for(int i=0; i<header.number_fields; ++i)
  {
    const uint16_t begin_name = fields_total_name_length;
    const uint16_t end_name = begin_name + field_descriptions[i].name_length;

    if(begin_name == end_name)
      throw QString("Corrupt property! (empty name)");
    if(end_name > joined_field_names.length())
      throw QString("Corrupt property! (buffer overflow)");

    const data_type::base_type_t base_type = field_descriptions[i].type;
    const QString name = QString::fromStdString(std::string(joined_field_names.data() + begin_name, joined_field_names.data() + end_name));

    if(!data_type::is_valid(base_type))
      throw QString("Corrupt property! (invalid type)");

    field_names << name;
    field_types << base_type;
    field_data_offset << fields_total_stride;

    fields_total_stride += data_type::size_of_type(base_type);
    fields_total_name_length += field_descriptions[i].name_length;
  }

  if(fields_total_stride != header.point_data_stride)
    throw QString("Corrupt header! (point data stride mismatch)");
  if(fields_total_name_length != header.field_names_total_size)
    throw QString("Corrupt header! (field names length mismatch)");

  pointcloud.aabb = header.aabb;
  pointcloud.resize(header.number_points);
  pointcloud.user_data_stride = header.point_data_stride;
  pointcloud.user_data_names = field_names;
  pointcloud.user_data_offset = field_data_offset;
  pointcloud.user_data_types = field_types;

  handle_loaded_chunk(current_progress += field_headers_size + field_names_size);

  if(load_vertex)
  {
    read_bytes = stream.readsome(reinterpret_cast<char*>(pointcloud.coordinate_color.data()), vertex_data_size);
    if(read_bytes != vertex_data_size)
      throw QString("Incomplete file!");
    handle_loaded_chunk(current_progress += vertex_data_size);
  }

  read_bytes = stream.readsome(reinterpret_cast<char*>(pointcloud.user_data.data()), point_data_size);
  if(read_bytes != point_data_size)
    throw QString("Incomplete file!");
  handle_loaded_chunk(current_progress += point_data_size);

  if(!load_vertex)
  {
    glm::ivec3 coord_src(-1);
    glm::ivec3 color_src(-1);

    for(int i=0; i<header.number_fields; ++i)
    {
      const QString& name = pointcloud.user_data_names[i];

      if(name == "x")
        coord_src.x = i;
      else if(name == "y")
        coord_src.y = i;
      else if(name == "z")
        coord_src.z = i;
      else if(name == "red")
        color_src.r = i;
      else if(name == "green")
        color_src.g = i;
      else if(name == "blue")
        color_src.b = i;
    }

    const uint8_t* user_data = pointcloud.user_data.data();

    auto read_property_as_float = [this, &user_data](int source_index) -> float32_t {
      return data_type::read_value_from_buffer<float32_t>(pointcloud.user_data_types[source_index], user_data + pointcloud.user_data_offset[source_index]);
    };

    auto read_property_as_uint8 = [this, &user_data](int source_index) -> uint8_t {
      return data_type::read_value_from_buffer<uint8_t>(pointcloud.user_data_types[source_index], user_data + pointcloud.user_data_offset[source_index]);
    };

    uint8_t* const coordinates = pointcloud.coordinate_color.data();

    size_t ui_update = 0;
    for(size_t i=0; i<header.number_points; ++i)
    {
      PointCloud::vertex_t vertex;

      vertex.coordinate.x = read_property_as_float(coord_src.x);
      vertex.coordinate.y = read_property_as_float(coord_src.y);
      vertex.coordinate.z = read_property_as_float(coord_src.z);
      vertex.color.r = read_property_as_uint8(color_src.x);
      vertex.color.g = read_property_as_uint8(color_src.y);
      vertex.color.b = read_property_as_uint8(color_src.z);

      write_value_to_buffer<PointCloud::vertex_t>(coordinates, vertex);

      if(Q_UNLIKELY(ui_update == 2048))
        handle_loaded_chunk(current_progress += ui_update);
      ui_update++;

      user_data += header.point_data_stride;
    }
    handle_loaded_chunk(current_progress += ui_update);
  }

  if(load_kd_tree)
  {
    read_bytes = stream.readsome(reinterpret_cast<char*>(pointcloud.kdtree_index.alloc_for_loading(header.number_points)), kd_tree_size);
    if(read_bytes != kd_tree_size)
      throw QString("Incomplete file!");
    handle_loaded_chunk(current_progress += kd_tree_size);
  }

  return true;
}
