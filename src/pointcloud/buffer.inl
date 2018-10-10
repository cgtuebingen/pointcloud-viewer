#include <pointcloud/buffer.hpp>
#include <iomanip>

namespace data_type {

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

template<typename stream_t>
size_t read_value_from_buffer_to_stream(stream_t& stream, base_type_t input_type, const uint8_t* input_buffer)
{
  switch(input_type)
  {
  case BASE_TYPE::FLOAT32:
  {
    auto value = ::read_value_from_buffer<float32_t>(input_buffer);
    stream << std::setprecision(std::numeric_limits<float32_t>::digits10) << value;
    return sizeof(value);
  }
  case BASE_TYPE::FLOAT64:
  {
    auto value = ::read_value_from_buffer<float64_t>(input_buffer);
    stream << std::setprecision(std::numeric_limits<float64_t>::digits10) << value;
    return sizeof(value);
  }
  case BASE_TYPE::INT8:
  {
    auto value = ::read_value_from_buffer<int8_t>(input_buffer);
    stream << int32_t(value);
    return sizeof(value);
  }
  case BASE_TYPE::INT16:
  {
    auto value = ::read_value_from_buffer<int16_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  case BASE_TYPE::INT32:
  {
    auto value = ::read_value_from_buffer<int32_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  case BASE_TYPE::INT64:
  {
    auto value = ::read_value_from_buffer<int64_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  case BASE_TYPE::UINT8:
  {
    auto value = ::read_value_from_buffer<uint8_t>(input_buffer);
    stream << uint32_t(value);
    return sizeof(value);
  }
  case BASE_TYPE::UINT16:
  {
    auto value = ::read_value_from_buffer<uint16_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  case BASE_TYPE::UINT32:
  {
    auto value = ::read_value_from_buffer<uint32_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  case BASE_TYPE::UINT64:
  {
    auto value = ::read_value_from_buffer<uint64_t>(input_buffer);
    stream << value;
    return sizeof(value);
  }
  }

  Q_UNREACHABLE();
  return 0;
}

inline size_t size_of_type(base_type_t base_type)
{
  switch(base_type)
  {
  case BASE_TYPE::FLOAT32:
    return 4;
  case BASE_TYPE::FLOAT64:
    return 8;
  case BASE_TYPE::INT8:
    return 1;
  case BASE_TYPE::INT16:
    return 2;
  case BASE_TYPE::INT32:
    return 4;
  case BASE_TYPE::INT64:
    return 8;
  case BASE_TYPE::UINT8:
    return 1;
  case BASE_TYPE::UINT16:
    return 2;
  case BASE_TYPE::UINT32:
    return 4;
  case BASE_TYPE::UINT64:
    return 8;
  }

  Q_UNREACHABLE();
  return 0;
}

inline bool is_valid(base_type_t base_type)
{
  switch(base_type)
  {
  case BASE_TYPE::FLOAT32:
  case BASE_TYPE::FLOAT64:
  case BASE_TYPE::INT8:
  case BASE_TYPE::INT16:
  case BASE_TYPE::INT32:
  case BASE_TYPE::INT64:
  case BASE_TYPE::UINT8:
  case BASE_TYPE::UINT16:
  case BASE_TYPE::UINT32:
  case BASE_TYPE::UINT64:
    return true;
  }

  return false;
}

} // namespace data_type
