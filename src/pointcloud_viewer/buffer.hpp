#ifndef POINTCLOUDVIEWER_BUFFER_HPP_
#define POINTCLOUDVIEWER_BUFFER_HPP_

#include <core_library/types.hpp>
#include <vector>

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

class Buffer final
{
public:
  Buffer();
  Buffer(Buffer&& other);
  void operator=(Buffer&& other);

  Buffer(const Buffer& buffer) = delete;
  Buffer& operator=(const Buffer& buffer) = delete;

  void clear();

  void fill(data_type_t internal_data_type, data_type_t input_data_type, const void* input_data, size_t input_size_in_bytes, size_t target_offset=0);

private:
  std::vector<uint8_t> bytes;
  data_type_t data_type;
  size_t num_elements;
};

#endif // POINTCLOUDVIEWER_BUFFER_HPP_
