#include <pointcloud/importer/ply_importer.hpp>
#include <pointcloud/convert_values.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <glm/gtx/io.hpp>

#include <QThread>
#include <QFileInfo>
#include <QAbstractEventDispatcher>

#include <iostream>

typedef pcl::io::ply::ply_parser ply_parser;

PlyImporter::PlyImporter(const std::string& input_file)
  : AbstractPointCloudImporter(input_file)
{
}

bool PlyImporter::import_implementation()
{
  current_progress = 0;
  vertex_data_stride = 0;
  property_names.clear();
  property_offsets.clear();
  property_types.clear();

  ply_parser parser;

  // initialze logging
  auto format_message = [this](const char* type, std::size_t line, const std::string& message) -> std::string {
    return QString("%0 %1 in line %2:%3")
        .arg(type)
        .arg(QFileInfo(QString::fromStdString(input_file)).fileName())
        .arg(line)
        .arg(QString::fromStdString(message))
        .toStdString();
  };
  parser.error_callback([format_message](std::size_t line, const std::string& message){print_error(format_message("Error while parsing ply file", line, message));});

  // == you may want to add those logging functions back in if you are debugging: ==
//  parser.warning_callback([format_message](std::size_t line, const std::string& message){println_error(format_message("Warning for ply file", line, message));});
//  parser.info_callback([format_message](std::size_t line, const std::string& message){println(format_message("Info: ", line, message));});

  PointCloud::vertex_t* new_vertex_x = nullptr;
  PointCloud::vertex_t* new_vertex_y = nullptr;
  PointCloud::vertex_t* new_vertex_z = nullptr;
  PointCloud::vertex_t* new_vertex_r = nullptr;
  PointCloud::vertex_t* new_vertex_g = nullptr;
  PointCloud::vertex_t* new_vertex_b = nullptr;
  uint8_t* all_data = nullptr;

  pointcloud.aabb = aabb_t::invalid();

  size_t num_points = std::numeric_limits<size_t>::max();

  // register the callback called after an element definition completed. Used to get the number of vertices, allocate teh buffers and prepare the pointers.
  parser.element_definition_callback([&num_points](const std::string& name, std::size_t n){
    if(name == "vertex")
      num_points = n;

    return ply_parser::element_callbacks_type();
  });

  // Register the callback for the individual property values
  ply_parser::scalar_property_definition_callbacks_type scalar_property_callbacks;

  ply_parser::at<uint8_t>(scalar_property_callbacks) = property_callback_handler<uint8_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<uint16_t>(scalar_property_callbacks) = property_callback_handler<uint16_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<uint32_t>(scalar_property_callbacks) = property_callback_handler<uint32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<int8_t>(scalar_property_callbacks) = property_callback_handler<int8_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<int16_t>(scalar_property_callbacks) = property_callback_handler<int16_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<int32_t>(scalar_property_callbacks) = property_callback_handler<int32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<float32_t>(scalar_property_callbacks) = property_callback_handler<float32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);
  ply_parser::at<float64_t>(scalar_property_callbacks) = property_callback_handler<float64_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b, &all_data);

  parser.scalar_property_definition_callbacks(scalar_property_callbacks);

  parser.end_header_callback([this, &num_points, &all_data, &new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b]() -> bool {
    this->pointcloud.set_user_data_format(vertex_data_stride, property_names, property_offsets, property_types);

    Q_ASSERT(num_points != std::numeric_limits<size_t>::max());

    // preallocate the necessary memory
    this->pointcloud.resize(num_points);

    // The pointers are used later for storing the actual vertex data
    new_vertex_x = reinterpret_cast<PointCloud::vertex_t*>(this->pointcloud.coordinate_color.data());
    new_vertex_y = new_vertex_x;
    new_vertex_z = new_vertex_x;
    new_vertex_r = new_vertex_x;
    new_vertex_g = new_vertex_x;
    new_vertex_b = new_vertex_x;

    Q_ASSERT(num_points < std::numeric_limits<int64_t>::max());
    this->total_progress = int64_t(num_points); // This slider is only used as a maximum value for the progress bar

    all_data = this->pointcloud.user_data.data();
    return true;
  });

  // Actually parse the file
  if(Q_UNLIKELY(!parser.parse(input_file)))
    return false;

  return true;
}


// Returns the callback handler for reading properties, which o the other hand returns the actual callback for each property depending on name and type.
template<typename value_type>
typename ply_parser::scalar_property_definition_callback_type<value_type>::type PlyImporter::property_callback_handler(PointCloud::vertex_t** new_vertex_x,
                                                                                                                       PointCloud::vertex_t** new_vertex_y,
                                                                                                                       PointCloud::vertex_t** new_vertex_z,
                                                                                                                       PointCloud::vertex_t** new_vertex_r,
                                                                                                                       PointCloud::vertex_t** new_vertex_g,
                                                                                                                       PointCloud::vertex_t** new_vertex_b,
                                                                                                                       uint8_t** all_data)
{
  return [new_vertex_x, new_vertex_y, new_vertex_z, new_vertex_r, new_vertex_g, new_vertex_b, all_data, this](const std::string& current_element_name, const std::string& property_name) -> typename ply_parser::scalar_property_callback_type<value_type>::type {
    if(current_element_name != "vertex")
      return ply_parser::scalar_property_callback_type<uint8_t>::type();

    const size_t offset = vertex_data_stride;
    property_names.append(QString::fromStdString(property_name));
    property_offsets.append(offset);
    property_types.append(data_type::base_type_of<value_type>::value());
    vertex_data_stride += sizeof(value_type);
    auto data_handler = [all_data](value_type value) {
      write_value_to_buffer(*all_data, value);
      *all_data += sizeof(value_type);
    };

    if(property_name == "red")
      return [new_vertex_r, data_handler](value_type value){data_handler(value);convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_r)++)->color.r);};
    if(property_name == "green")
      return [new_vertex_g, data_handler](value_type value){data_handler(value);convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_g)++)->color.g);};
    if(property_name == "blue")
      return [new_vertex_b, data_handler](value_type value){data_handler(value);convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_b)++)->color.b);};

    aabb_t& aabb = this->pointcloud.aabb;

    if(property_name == "x")
      return [new_vertex_x, this, &aabb, data_handler](value_type value){
        data_handler(value);
        float x;
        convert_component<value_type, float32_t>::convert_normalized(&value, &x);
        ((*new_vertex_x)++)->coordinate.x = x;

        aabb.max_point.x = glm::max(x, aabb.max_point.x);
        aabb.min_point.x = glm::min(x, aabb.min_point.x);

        // Update the progress bar to relax the user.
        current_progress++;
        if(Q_UNLIKELY(current_progress%4096 == 0))
          handle_loaded_chunk(current_progress);
      };
    if(property_name == "y")
      return [new_vertex_y, &aabb, data_handler](value_type value){
        data_handler(value);
        float y;
        convert_component<value_type, float32_t>::convert_normalized(&value, &y);
        ((*new_vertex_y)++)->coordinate.y = y;

        aabb.max_point.y = glm::max(y, aabb.max_point.y);
        aabb.min_point.y = glm::min(y, aabb.min_point.y);
      };
    if(property_name == "z")
      return [new_vertex_z, &aabb, data_handler](value_type value){
        data_handler(value);
        float z;
        convert_component<value_type, float32_t>::convert_normalized(&value, &z);
        ((*new_vertex_z)++)->coordinate.z = z;

        aabb.max_point.z = glm::max(z, aabb.max_point.z);
        aabb.min_point.z = glm::min(z, aabb.min_point.z);
      };

    return data_handler;
  };
};
