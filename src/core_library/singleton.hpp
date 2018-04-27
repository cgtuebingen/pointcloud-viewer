#include <assert.h>

template<typename T>
class Singleton
{
public:
  Singleton()
  {
    assert(_global_instance() == nullptr && "There can be only one instance of Instance");
    _global_instance() = this;
  }

  ~Singleton()
  {
    assert(_global_instance() == this && "There can be only one instance of Instance");
    _global_instance() = nullptr;
  }

  Singleton(const Singleton&) = delete;
  void operator=(const Singleton&) = delete;

private:
  static Singleton<T>*& _global_instance()
  {
    static Singleton<T>* instance = nullptr;

    return instance;
  }
};
