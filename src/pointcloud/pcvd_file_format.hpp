#include <pointcloud/pointcloud.hpp>

namespace pcvd_format {

/*
The file format consists out of the following sections (plain binary data without any padding in between):

  HEADER                    // Just header_t from below
  FIELD_DESCRIPTION         // an array of field_description_t[header.number_fields]
  FIELD_NAMES               // ascii string with the length of header.field_names_total_size. Content consists out of the field names in each field description
  POINT_CLOUD_VERTEX_DATA   // optional - existant if and only if `(flags & 0b10)!=0` array of vertex_t[header.number_points]
  POINT_CLOUD_DATA          // mandatory, must have the size point_data_stride * number_points. Format is described by  the field headers
  KD_TREE                   // optional - existant if and only if `(flags & 0b1)!=0`. array uint64_t[header.number_points]
  UNKNOWN_DATA              // optional, only allowed if and only if `(flags&0xfc)!=0`)
*/

struct header_t
{
  static constexpr uint32_t expected_macic_number() {
    return (((uint32_t('p')<<8) | uint32_t('c')<<8) | uint32_t('v')<<8) | uint32_t('d'); // point cloud viewer dump
  }

  uint32_t magic_number; // must be `expected_macic_number()`

  uint16_t file_version_number; // the file version
  uint16_t downwards_compatibility_version_number; // up to which file version is this file downwards compatible

  uint64_t number_points; // total number of points
  uint32_t point_data_stride; // must be equal to the sum of the sizes of all field_description_t::size

  uint16_t field_names_total_size; // must be equal to the sum of all field_description_t::name_length
  uint8_t number_fields; // total number of fields

  uint8_t flags; // 0b1: contains kdtree, 0b10: contains vertex_data other bits must be zero.

  aabb_t aabbt;

  uint64_t reserved; // ignored
};

struct field_description_t
{
  uint8_t name_length;
  data_type::base_type_t type;
};

} // namespace pcvd_format
