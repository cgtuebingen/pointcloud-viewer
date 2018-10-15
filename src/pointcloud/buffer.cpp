#include <pointcloud/buffer.hpp>
#include <pointcloud/convert_values.hpp>
#include <core_library/print.hpp>

#include <QString>
#include <glm/glm.hpp>

Buffer::Buffer()
{
}

Buffer::Buffer(Buffer&& other) = default;

Buffer& Buffer::operator=(Buffer&& other) = default;

uint8_t*Buffer::data()
{
  return bytes.data();
}

const uint8_t*Buffer::data() const
{
  return bytes.data();
}

void Buffer::clear()
{
  bytes.clear();
}

void Buffer::resize(size_t size)
{
  bytes.resize(size);
}

void Buffer::memset(uint32_t value)
{
  std::memset(bytes.data(), int(value), bytes.size());
}

namespace data_type
{

QString toString(data_type::base_type_t base_type)
{
  switch(base_type)
  {
  case BASE_TYPE::INT8:
    return "int8_t";
  case BASE_TYPE::INT16:
    return "int16_t";
  case BASE_TYPE::INT32:
    return "int32_t";
  case BASE_TYPE::INT64:
    return "int64_t";
  case BASE_TYPE::UINT8:
    return "uint8_t";
  case BASE_TYPE::UINT16:
    return "uint16_t";
  case BASE_TYPE::UINT32:
    return "uint_t";
  case BASE_TYPE::UINT64:
    return "uint64_t";
  case BASE_TYPE::FLOAT32:
    return "float32_t";
  case BASE_TYPE::FLOAT64:
    return "float64_t";
  }

  Q_UNREACHABLE();
  return "<unknown type>";
}

} // namespace data_type
