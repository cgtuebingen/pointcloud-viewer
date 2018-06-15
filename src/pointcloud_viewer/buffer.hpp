#ifndef POINTCLOUDVIEWER_BUFFER_HPP_
#define POINTCLOUDVIEWER_BUFFER_HPP_

#include <core_library/types.hpp>
#include <vector>

/**
Data Type of the values stored in the buffer.

The Data type consists out of the
- base type ((u)int8/16/32/64 or float32/64). Normalized means, that the possible range gets remapped to [0...1]
- number of components
- stride in bytes

The components are tightly packed and the stride larger than the components together results in padding between two vertices.
For example data_type_t{FLOAT32, 3, 4*4} results in a buffer looking like this:

 [4 bytes for first float of the first vertex]
 [4 bytes for second float of the first vertex]
 [4 bytes for third float of the first vertex]
 [4 padding]
 [4 bytes for first float of the second vertex]
 [4 bytes for second float of the second vertex]
 [4 bytes for third float of the second vertex]
 [4 padding]
 [4 bytes for first float of the third vertex]
 ...
*/
struct data_type_t final
{
  enum class base_type_t : uint8_t
  {
    INT8_NORMALIZED,
    INT16_NORMALIZED,
    INT32_NORMALIZED,
    INT64_NORMALIZED,
    UINT8_NORMALIZED,
    UINT16_NORMALIZED,
    UINT32_NORMALIZED,
    UINT64_NORMALIZED,
    FLOAT32,
    FLOAT64,
  };
  typedef base_type_t BASE_TYPE;

  base_type_t base_type;
  uint num_components;
  uint stride_in_bytes;
};

/**
Buffer for storing the point cloud.
*/
class Buffer final
{
public:
  Buffer();
  Buffer(Buffer&& other);
  void operator=(Buffer&& other);

  Buffer(const Buffer& buffer) = delete;
  Buffer& operator=(const Buffer& buffer) = delete;

  const uint8_t* data() const;

  void clear();

  // Note, that the padding will be left untouched, so you can call ths function
  // multiple times to store multiple array withing the same buffer.
  // The point clouds for example use the first 12 bytes for a vec3 coordinate
  // and from the 4 bytes padding, the first 3 are in fact used as u8vec3 color.
  void fill(data_type_t internal_data_type, data_type_t input_data_type, const uint8_t* input_data, size_t input_size_in_bytes, size_t target_offset=0);

private:
  std::vector<uint8_t> bytes;
  data_type_t data_type;
  size_t num_elements;
};

#endif // POINTCLOUDVIEWER_BUFFER_HPP_
