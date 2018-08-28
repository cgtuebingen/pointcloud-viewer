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
