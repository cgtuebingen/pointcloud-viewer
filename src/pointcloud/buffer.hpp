#ifndef POINTCLOUDVIEWER_BUFFER_HPP_
#define POINTCLOUDVIEWER_BUFFER_HPP_

#include <core_library/types.hpp>
#include <vector>
#include <QtGlobal>

namespace data_type {

enum class base_type_t : uint8_t
{
  INT8,
  INT16,
  INT32,
  INT64,
  UINT8,
  UINT16,
  UINT32,
  UINT64,
  FLOAT32,
  FLOAT64,
};
typedef base_type_t BASE_TYPE;

template<typename T>
struct base_type_of;

template<typename T>
T read_value_from_buffer(base_type_t input_type, const uint8_t* input_buffer);

#define BASE_TYPE(c_type, enum_value) template<>struct base_type_of<c_type>{static constexpr base_type_t value(){return base_type_t::enum_value;}};
BASE_TYPE(int8_t, INT8)
BASE_TYPE(int16_t, INT16)
BASE_TYPE(int32_t, INT32)
BASE_TYPE(int64_t, INT64)
BASE_TYPE(uint8_t, UINT8)
BASE_TYPE(uint16_t, UINT16)
BASE_TYPE(uint32_t, UINT32)
BASE_TYPE(uint64_t, UINT64)
BASE_TYPE(float32_t, FLOAT32)
BASE_TYPE(float64_t, FLOAT64)
#undef BASE_TYPE


template<typename T>
T read_value_from_buffer(base_type_t input_type, const uint8_t* input_buffer)
{
  switch(input_type)
  {
  case BASE_TYPE::FLOAT32:
    return ::read_value_from_buffer<float32_t>(input_buffer);
  case BASE_TYPE::FLOAT64:
    return ::read_value_from_buffer<float64_t>(input_buffer);
  case BASE_TYPE::INT8:
    return ::read_value_from_buffer<int8_t>(input_buffer);
  case BASE_TYPE::INT16:
    return ::read_value_from_buffer<int16_t>(input_buffer);
  case BASE_TYPE::INT32:
    return ::read_value_from_buffer<int32_t>(input_buffer);
  case BASE_TYPE::INT64:
    return ::read_value_from_buffer<int64_t>(input_buffer);
  case BASE_TYPE::UINT8:
    return ::read_value_from_buffer<uint8_t>(input_buffer);
  case BASE_TYPE::UINT16:
    return ::read_value_from_buffer<uint16_t>(input_buffer);
  case BASE_TYPE::UINT32:
    return ::read_value_from_buffer<uint32_t>(input_buffer);
  case BASE_TYPE::UINT64:
    return ::read_value_from_buffer<uint64_t>(input_buffer);
  }

  Q_UNREACHABLE();
  return T();
}

} // namespace data_type

/**
Buffer for storing the point cloud.
*/
class Buffer final
{
public:
  Buffer();
  Buffer(Buffer&& other);
  Buffer& operator=(Buffer&& other);

  Buffer(const Buffer& buffer) = delete;
  Buffer& operator=(const Buffer& buffer) = delete;

  uint8_t* data();
  const uint8_t* data() const;

  void clear();

  void resize(size_t size);
  void memset(uint32_t value);

private:
  std::vector<uint8_t> bytes;
};

#endif // POINTCLOUDVIEWER_BUFFER_HPP_
