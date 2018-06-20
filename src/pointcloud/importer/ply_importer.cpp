#include <pointcloud/importer/ply_importer.hpp>
#include <pointcloud/convert_values.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <pcl/io/ply/ply_parser.h>

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

// Returns the callback handler for reading properties, which o the other hand returns the actual callback for each property depending on name and type.
template<typename value_type>
typename ply_parser::scalar_property_callback_type<value_type>::type property_callback_handler(PointCloud::vertex_t** new_vertex);

bool PlyImporter::import_implementation()
{

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
  parser.warning_callback([format_message](std::size_t line, const std::string& message){print_error(format_message("Warning for ply file", line, message));});
  parser.info_callback([format_message](std::size_t line, const std::string& message){print(format_message("Info: ", line, message));});

  size_t num_vertices = 0;
  PointCloud::vertex_t* new_vertex = nullptr; // TODO::::::: actually use

  parser.element_definition_callback([&num_vertices](const std::string& name, std::size_t n){
    if(name != "vertex")
      return ply_parser::element_callbacks_type();
    num_vertices = n;
    return ply_parser::element_callbacks_type();
  });

  ply_parser::scalar_property_definition_callbacks_type scalar_property_callbacks;

  ply_parser::at<uint8_t>(scalar_property_callbacks) = property_callback_handler<uint8_t>(&new_vertex);
  ply_parser::at<uint16_t>(scalar_property_callbacks) = property_callback_handler<uint16_t>(&new_vertex);
  ply_parser::at<uint32_t>(scalar_property_callbacks) = property_callback_handler<uint32_t>(&new_vertex);
  ply_parser::at<uint64_t>(scalar_property_callbacks) = property_callback_handler<uint64_t>(&new_vertex);
  ply_parser::at<int8_t>(scalar_property_callbacks) = property_callback_handler<int8_t>(&new_vertex);
  ply_parser::at<int16_t>(scalar_property_callbacks) = property_callback_handler<int16_t>(&new_vertex);
  ply_parser::at<int32_t>(scalar_property_callbacks) = property_callback_handler<int32_t>(&new_vertex);
  ply_parser::at<int64_t>(scalar_property_callbacks) = property_callback_handler<int64_t>(&new_vertex);
  ply_parser::at<float32_t>(scalar_property_callbacks) = property_callback_handler<float32_t>(&new_vertex);
  ply_parser::at<float64_t>(scalar_property_callbacks) = property_callback_handler<float64_t>(&new_vertex);

  parser.scalar_property_definition_callbacks(scalar_property_callbacks);

  if(Q_UNLIKELY(!parser.parse(input_file)))
    return false;

  print_error("TODO");
  return false;
}


// Returns the callback handler for reading properties, which o the other hand returns the actual callback for each property depending on name and type.
template<typename value_type>
typename ply_parser::scalar_property_callback_type<value_type>::type property_callback_handler(PointCloud::vertex_t** new_vertex)
{
  return [new_vertex](const std::string& current_element_name, const std::string& property_name) -> typename ply_parser::scalar_property_callback_type<uint8_t>::type {
    if(current_element_name != "vertex")
      return ply_parser::scalar_property_callback_type<uint8_t>::type();

    if(property_name == "red")
      return [&new_vertex](uint8_t value){convert_component<value_type, uint8_t>::convert_normalized(&value, &(*new_vertex)->color.r);};
    if(property_name == "green")
      return [&new_vertex](uint8_t value){convert_component<value_type, uint8_t>::convert_normalized(&value, &(*new_vertex)->color.g);};
    if(property_name == "blue")
      return [&new_vertex](uint8_t value){convert_component<value_type, uint8_t>::convert_normalized(&value, &(*new_vertex)->color.b);};

    if(property_name == "x")
      return [&new_vertex](uint8_t value){convert_component<value_type, float32_t>::convert_normalized(&value, &(*new_vertex)->coordinate.x);};
    if(property_name == "y")
      return [&new_vertex](uint8_t value){convert_component<value_type, float32_t>::convert_normalized(&value, &(*new_vertex)->coordinate.y);};
    if(property_name == "z")
      return [&new_vertex](uint8_t value){convert_component<value_type, float32_t>::convert_normalized(&value, &(*new_vertex)->coordinate.z);};

    print_error("Warning: Property ", property_name, " ignored! Data components are currently not supported.");
    return ply_parser::scalar_property_callback_type<uint8_t>::type();
  };
};
