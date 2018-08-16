#ifndef CORELIBRARY_STACK_HPP_
#define CORELIBRARY_STACK_HPP_

#include <vector>

template<typename T>
class Stack
{
public:
  std::vector<T> values;

  void reserve(size_t length);

  bool is_empty() const;

  void push(const T& value);

  T pop();
};

#include <core_library/stack.inl>

#endif // CORELIBRARY_STACK_HPP_
