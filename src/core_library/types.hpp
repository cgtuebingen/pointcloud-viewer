#ifndef CORELIBRARY_TYPES_HPP_
#define CORELIBRARY_TYPES_HPP_

#include <stdint.h>
#include <cstddef>

#include <cstring>

typedef long double float86_t;
typedef double float64_t;
typedef float float32_t;

typedef unsigned int uint;

template<typename T_out, typename T_in>
T_out read_value_from_buffer(const T_in* buffer)
{
  T_out value;
  std::memcpy(&value, buffer, sizeof(T_out));
  return value;
}

template<typename T_in, typename T_out>
void write_value_to_buffer(T_out* buffer, const T_in value)
{
  std::memcpy(buffer, &value, sizeof(T_in));
}


#endif // CORELIBRARY_TYPES_HPP_
