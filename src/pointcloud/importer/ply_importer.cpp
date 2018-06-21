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

PlyImporter::PlyImporter(const std::string& input_file, int64_t total_num_bytes)
  : AbstractPointCloudImporter(input_file, total_num_bytes)
{
}

bool PlyImporter::import_implementation()
{
  current_progress = 0;

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
//  parser.warning_callback([format_message](std::size_t line, const std::string& message){print_error(format_message("Warning for ply file", line, message));});
//  parser.info_callback([format_message](std::size_t line, const std::string& message){print(format_message("Info: ", line, message));});

  PointCloud::vertex_t* new_vertex_x = nullptr;
  PointCloud::vertex_t* new_vertex_y = nullptr;
  PointCloud::vertex_t* new_vertex_z = nullptr;
  PointCloud::vertex_t* new_vertex_r = nullptr;
  PointCloud::vertex_t* new_vertex_g = nullptr;
  PointCloud::vertex_t* new_vertex_b = nullptr;

  // register the callback called after an element definition completed. Used to get the number of vertices, allocate teh buffers and prepare the pointers.
  parser.element_definition_callback([this, &new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b](const std::string& name, std::size_t n){
    if(name != "vertex")
      return ply_parser::element_callbacks_type();

    // preallocate the necessary memory
    this->pointcloud.resize(n);

    // The pointers are used later for storing the actual vertex data
    new_vertex_x = reinterpret_cast<PointCloud::vertex_t*>(this->pointcloud.coordinate_color.data());
    new_vertex_y = new_vertex_x;
    new_vertex_z = new_vertex_x;
    new_vertex_r = new_vertex_x;
    new_vertex_g = new_vertex_x;
    new_vertex_b = new_vertex_x;

    Q_ASSERT(n < std::numeric_limits<int64_t>::max());
    this->total_progress = int64_t(n); // This slider is only used as a maximum value for the progress bar
    return ply_parser::element_callbacks_type();
  });

  // Register the callback for the individual property values
  ply_parser::scalar_property_definition_callbacks_type scalar_property_callbacks;

  ply_parser::at<uint8_t>(scalar_property_callbacks) = property_callback_handler<uint8_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<uint16_t>(scalar_property_callbacks) = property_callback_handler<uint16_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<uint32_t>(scalar_property_callbacks) = property_callback_handler<uint32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<int8_t>(scalar_property_callbacks) = property_callback_handler<int8_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<int16_t>(scalar_property_callbacks) = property_callback_handler<int16_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<int32_t>(scalar_property_callbacks) = property_callback_handler<int32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<float32_t>(scalar_property_callbacks) = property_callback_handler<float32_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);
  ply_parser::at<float64_t>(scalar_property_callbacks) = property_callback_handler<float64_t>(&new_vertex_x, &new_vertex_y, &new_vertex_z, &new_vertex_r, &new_vertex_g, &new_vertex_b);

  parser.scalar_property_definition_callbacks(scalar_property_callbacks);

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
                                                                                                                       PointCloud::vertex_t** new_vertex_b)
{
  return [new_vertex_x, new_vertex_y, new_vertex_z, new_vertex_r, new_vertex_g, new_vertex_b, this](const std::string& current_element_name, const std::string& property_name) -> typename ply_parser::scalar_property_callback_type<value_type>::type {
    if(current_element_name != "vertex")
      return ply_parser::scalar_property_callback_type<uint8_t>::type();

    if(property_name == "red")
      return [new_vertex_r](value_type value){convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_r)++)->color.r);};
    if(property_name == "green")
      return [new_vertex_g](value_type value){convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_g)++)->color.g);};
    if(property_name == "blue")
      return [new_vertex_b](value_type value){convert_component<value_type, uint8_t>::convert_normalized(&value, &((*new_vertex_b)++)->color.b);};

    if(property_name == "x")
      return [new_vertex_x, this](value_type value){
        convert_component<value_type, float32_t>::convert_normalized(&value, &((*new_vertex_x)++)->coordinate.x);

        // Update the progress bar to relax the user.
        current_progress++;
        if(Q_UNLIKELY(current_progress%4096 == 0))
          handle_loaded_chunk(current_progress);
      };
    if(property_name == "y")
      return [new_vertex_y](value_type value){convert_component<value_type, float32_t>::convert_normalized(&value, &((*new_vertex_y)++)->coordinate.y);};
    if(property_name == "z")
      return [new_vertex_z](value_type value){convert_component<value_type, float32_t>::convert_normalized(&value, &((*new_vertex_z)++)->coordinate.z);};

    print_error("Warning: Property ", property_name, " ignored! Data components are currently not supported.");
    return ply_parser::scalar_property_callback_type<uint8_t>::type();
  };
};
