#include <pointcloud_viewer/buffer.hpp>
#include <cstring>

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

void Buffer::clear()
{
  bytes.clear();
}

struct copy_parameters_t
{
  uint8_t* target_array;
  size_t target_stride;
  const void* source_array;
  size_t source_stride;
  size_t num_elements;
  size_t num_components;
};

/*
void to_float(int8_t value)

template<typename t_in, typename t_out>
struct convert_component
{
  static void convert(const void* source, uint8_t* target)
  {
    t_in value_in;
    t_out value_out;

    std::memcpy(&value_in, source, sizeof(t_in));

    value_in
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
        convert_component<t_in, t_out>::convert(p.source_array+sizeof(t_in)*component_index, p.target_array+sizeof(t_out)*component_index);
      }
    }
  }
};
*/

void Buffer::fill(data_type_t internal_data_type, data_type_t input_data_type, const void* data, size_t input_size_in_bytes, size_t target_offset)
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
