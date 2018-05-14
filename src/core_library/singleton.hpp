#ifndef CORELIBRARY_SINGLETON_HPP_
#define CORELIBRARY_SINGLETON_HPP_

#include <assert.h>

#include <core_library/macros.hpp>

/*
Use this class as a base class for any class, you want to have only one
instance.

Note, that the global instance is only used for validation, but is not
accessible itself. That's, because the class is not meant to be accessed
directly, but to initialize/deinitialize the necessary resources in a raii way.
For accessing the actual data, static functions are meant to be used. See the
mainwindow namespace for an example
*/
template<typename T>
class Singleton
{
public:
  nocopy(Singleton)

  Singleton()
  {
    assert(_global_instance() == nullptr && "There can be only one singleton instance of this type.");
    _global_instance() = this;
  }

  ~Singleton()
  {
    assert(_global_instance() == this && "Internal error! Somehow, someone managed to create a second singleton instance of this type.");
    _global_instance() = nullptr;
  }

  static bool is_initalized()
  {
    return _global_instance() != nullptr;
  }

private:
  static Singleton<T>*& _global_instance()
  {
    static Singleton<T>* instance = nullptr;

    return instance;
  }
};

#endif // CORELIBRARY_SINGLETON_HPP_
