#include <core_library/stack.hpp>
#include <QtGlobal>

template<typename T>
void Stack<T>::reserve(size_t length)
{
  values.reserve(length);
}

template<typename T>
bool Stack<T>::is_empty() const
{
  return values.empty();
}

template<typename T>
void Stack<T>::push(const T& value)
{
  values.push_back(value);
}

template<typename T>
T Stack<T>::pop()
{
  Q_ASSERT(!is_empty());

  T value = std::move(*(values.end()-1));
  values.pop_back();
  return value;
}
