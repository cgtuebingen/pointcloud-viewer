#include <pointcloud/buffer.hpp>
#include <pointcloud/convert_values.hpp>
#include <core_library/print.hpp>

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
}

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

struct copy_parameters_t
{
  uint8_t* target_array;
  size_t target_stride;
  const uint8_t* source_array;
  size_t source_stride;
  size_t num_elements;
  size_t num_components;
};

template<typename t_in, typename t_out>
struct buffer_copy
{
  static void copy_normalized(const copy_parameters_t p)
  {
    const uint8_t* source_array = p.source_array;
    uint8_t* target_array = p.target_array;

    for(size_t element_index=0; element_index<p.num_elements; ++element_index)
    {
      const uint8_t* source_component = source_array;
      uint8_t* target_component = target_array;

      for(size_t component_index=0; component_index<p.num_components; ++component_index)
      {
        convert_component<t_in, t_out>::convert_normalized(source_component, target_component);

        source_component += sizeof(t_in);
        target_component += sizeof(t_out);
      }

      source_array += p.source_stride;
      target_array += p.target_stride;
    }
  }
};

void Buffer::fill(data_type_t internal_data_type, data_type_t input_data_type, const uint8_t* data, size_t input_size_in_bytes, size_t target_offset)
{
  this->data_type = internal_data_type;

  if((input_size_in_bytes % input_data_type.stride_in_bytes) != 0)
    throw QString("Size mismatch between the data type and the given buffer.");

  size_t num_elements = input_size_in_bytes/input_data_type.stride_in_bytes;

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
    break; \
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
