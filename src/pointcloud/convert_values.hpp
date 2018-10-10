#ifndef POINTCLOUDVIEWER_CONVERT_VALUES_HPP_
#define POINTCLOUDVIEWER_CONVERT_VALUES_HPP_

#include <core_library/types.hpp>
#include <type_traits>
#include <limits>

#include <glm/glm.hpp>

template<size_t size>
struct float_type
{
  typedef float32_t type;
};

template<>
struct float_type<8> // 8 byte = 64 bit
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
  static void convert_absolute(const void* source, void* target)
  {
    typedef typename float_type<sizeof(t_out)>::type float_t;

    t_in in_value = read_value_from_buffer<t_in>(source);

    if(std::is_floating_point<t_in>::value && !std::is_floating_point<t_out>::value)
      in_value = glm::round(in_value);

    in_value = glm::clamp<t_in>(in_value,
                                t_in(std::numeric_limits<t_out>::min()),
                                t_in(std::numeric_limits<t_out>::max()));

    t_out out_value = t_out(in_value);

    write_value_to_buffer(target, out_value);
  }

  static void convert_normalized(const void* source, void* target)
  {
    typedef typename float_type<sizeof(t_out)>::type float_t;
    write_value_to_buffer(target, from_float_normalized<t_out>(static_cast<float_t>(to_float_normalized<t_in>(read_value_from_buffer<t_in>(source)))));
  }
};

template<typename T>
struct convert_component<T, T>
{
  static void convert_absolute(const void* source, void* target)
  {
    std::memcpy(target, source, sizeof(T));
  }

  static void convert_normalized(const void* source, void* target)
  {
    convert_absolute(source, target);
  }
};

#endif // POINTCLOUDVIEWER_CONVERT_VALUES_HPP_
