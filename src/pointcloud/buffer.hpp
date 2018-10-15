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

template<typename stream_t>
size_t read_value_from_buffer_to_stream(stream_t& stream, base_type_t input_type, const uint8_t* input_buffer);

size_t size_of_type(base_type_t base_type);
bool is_valid(base_type_t base_type);

QString toString(base_type_t base_type);

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

#include <pointcloud/buffer.inl>

#endif // POINTCLOUDVIEWER_BUFFER_HPP_
