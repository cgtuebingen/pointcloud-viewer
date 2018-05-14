#ifndef CORELIBRARY_MACROS_HPP_
#define CORELIBRARY_MACROS_HPP_

#if defined(__GNUC__) || defined(__clang__)
#define GCC_OR_CLANG 1
#else
#define GCC_OR_CLANG 0
#endif



#if GCC_OR_CLANG

#define LIKELY(x) __builtin_expect(bool(x), 1)
#define UNLIKELY(x) __builtin_expect(bool(x), 0)

#else

#define LIKELY(x)
#define UNLIKELY(x)

#endif



#define nocopy(name) \
  name(const name&) = delete; \
  name& operator=(const name&) = delete; \



#endif // CORELIBRARY_MACROS_HPP_
