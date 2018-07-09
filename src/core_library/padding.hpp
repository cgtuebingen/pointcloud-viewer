#ifndef CORELIBRARY_PADDING_HPP_
#define CORELIBRARY_PADDING_HPP_

#include <stddef.h>
#include <stdint.h>

/*
Helper class to make padding more obvious.

Example usage:

 vec3 interesting_part1;
 padding<float> _padding1;
 vec3 interesting_part2;
 padding<float> _padding2;
*/

template<typename T, int N=1>
struct padding final
{
public:
  void clear()
  {
    size_t s = sizeof(T)*N;
    for(size_t i=0; i<s; ++i)
      _[i] = 0xcc;
  }

private:
  uint8_t _[sizeof(T)*N];
};

#endif // CORELIBRARY_PADDING_HPP_
