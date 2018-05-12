#include <core_library/string.hpp>

#include <iostream>

namespace __internal__
{

inline void format(std::ostream&)
{
}

template<typename arg_t, typename... other_arg_t>
void format(std::ostream& stream, const arg_t& arg, const other_arg_t&... args)
{
  stream << arg;
  format(stream, args...);
}

template<typename stream_t, typename... other_arg_t>
stream_t& format_implementation(stream_t& stream, const other_arg_t&... args)
{
  __internal__::format(stream, args...);

  return stream;
}


} // __internal__

template<typename... arg_t>
std::string format(const arg_t&... args)
{
  std::ostringstream stream;

  return __internal__::format_implementation(stream, args...).str();
}


template<typename... arg_t>
std::ostream& print(const arg_t&... args)
{
  return __internal__::format_implementation(std::cout, args...) << std::endl;
}

template<typename... arg_t>
std::ostream& print_error(const arg_t&... args)
{
  return __internal__::format_implementation(std::cerr, args...) << std::endl;
}
