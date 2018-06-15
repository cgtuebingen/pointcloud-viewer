#include <pointcloud_viewer/buffer.hpp>

#include <QString>
#include <glm/glm.hpp>

typedef data_type_t::BASE_TYPE BASE_TYPE;


Buffer::Buffer()
{
}

Buffer::Buffer(Buffer&& other)
  : Buffer()
{
  *this = std::move(other);
}

void Buffer::operator=(Buffer&& other)
{
  bytes.swap(other.bytes);
  std::swap(data_type, other.data_type);
  std::swap(num_elements, other.num_elements);
}

const uint8_t*Buffer::data() const
{
  return bytes.data();
}

void Buffer::clear()
{
  bytes.clear();
}

struct copy_parameters_t
{
  uint8_t* target_array;
  size_t target_stride;
  const uint8_t* source_array;
  size_t source_stride;
  size_t num_elements;
  size_t num_components;
};

template<size_t size>
struct float_type
{
  typedef float32_t type;
};

template<>
struct float_type<8>
{
  typedef float64_t type;
};

template<typename T, typename float_t=typename float_type<sizeof(T)>::type>
typename std::enable_if<std::is_integral<T>::value && !std::is_signed<T>::value, float_t>::type
to_float_normalized(T value)
{
  return float_t(value) / float_t(std::numeric_limits<T>::max());
}

template<typename T, typename float_t=typename float_type<sizeof(T)>::type>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, float_t>::type
to_float_normalized(T value)
{
  if(value >= 0)
    return float_t(value) / float_t(std::numeric_limits<T>::max());
  else
    return -float_t(value) / float_t(std::numeric_limits<T>::min());
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
to_float_normalized(T value)
{
  return value;
}

template<typename T, typename float_t=typename float_type<sizeof(T)>::type>
typename std::enable_if<std::is_integral<T>::value && !std::is_signed<T>::value, T>::type
from_float_normalized(float_t value)
{
  value = value * float_t(std::numeric_limits<T>::max());

  return T(glm::clamp<float_t>(glm::round(value),
                               0,
                               float_t(std::numeric_limits<T>::max())));
}

template<typename T, typename float_t=typename float_type<sizeof(T)>::type>
typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value, T>::type
from_float_normalized(float_t value)
{
  if(value >= 0)
    value = value * float_t(std::numeric_limits<T>::max());
  else
    value = -value * float_t(std::numeric_limits<T>::min());

  return T(glm::clamp(glm::round(value),
                      float_t(std::numeric_limits<T>::min()),
                      float_t(std::numeric_limits<T>::max())));
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
from_float_normalized(T value)
{
  return value;
}

template<typename t_in, typename t_out>
struct convert_component
{
  static void convert_normalized(const void* source, uint8_t* target)
  {
    typedef typename float_type<sizeof(t_out)>::type float_t;
    write_value_to_buffer(target, from_float_normalized<t_out>(static_cast<float_t>(to_float_normalized<t_in>(read_value_from_buffer<t_in>(source)))));
  }
};

template<typename T>
struct convert_component<T, T>
{
  static void convert_normalized(const void* source, uint8_t* target)
  {
    std::memcpy(target, source, sizeof(T));
  }
};

template<typename t_in, typename t_out>
struct buffer_copy
{
  static void copy_normalized(copy_parameters_t p)
  {
    for(size_t element_index=0; element_index<p.num_elements; ++element_index)
    {
      for(size_t component_index=0; component_index<p.num_components; ++component_index)
      {
        convert_component<t_in, t_out>::convert_normalized(p.source_array+sizeof(t_in)*component_index, p.target_array+sizeof(t_out)*component_index);
      }
    }
  }
};

void Buffer::fill(data_type_t internal_data_type, data_type_t input_data_type, const uint8_t* data, size_t input_size_in_bytes, size_t target_offset)
{
  this->data_type = internal_data_type;

  if((input_size_in_bytes % input_data_type.stride_in_bytes) != 0)
    throw QString("Size mismatch between the data type and the given buffer.");

  num_elements = input_size_in_bytes/input_data_type.stride_in_bytes;

  size_t target_size_in_bytes = internal_data_type.stride_in_bytes * num_elements;

  if(bytes.size() != target_size_in_bytes)
    bytes.resize(target_size_in_bytes);

  copy_parameters_t p;
  p.target_array = bytes.data() + target_offset;
  p.target_stride = internal_data_type.stride_in_bytes;
  p.source_array = data;
  p.source_stride = input_data_type.stride_in_bytes;
  p.num_elements = num_elements;
  p.num_components = glm::min(internal_data_type.num_components, input_data_type.num_components);

#define BEGIN_TYPE_COMBI(TYPE, t) \
  case BASE_TYPE::TYPE: \
  { \
  typedef t output_type_t; \
  switch(input_data_type.base_type) \
  {

#define TYPE(TYPE, t) \
  case BASE_TYPE::TYPE:  { \
  typedef t input_type_t; \
    buffer_copy<input_type_t, output_type_t>::copy_normalized(p); \
  }

#define END_TYPE_COMBI \
  } \
  break; \
  }

  switch(internal_data_type.base_type)
  {
#include "./helpers/all_buffer_types_combi.inl"
  }

#undef BEGIN_TYPE_COMBI
#undef TYPE
#undef END_TYPE_COMBI

}
