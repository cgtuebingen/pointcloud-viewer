#ifndef CORELIBRARY_PRINT_HPP_
#define CORELIBRARY_PRINT_HPP_

#include <string>
#include <sstream>

/*
Helper functions for operating with strings.

`format` helps formatting a string. For example

    format("answer to the life, universe and everything: ", 42, ". Deep inside, you know it's true!")

`print` do the same, but they already prints the result to the console.
*/

template<typename... arg_t>
std::string format(const arg_t&... args);

template<typename... arg_t>
std::ostream& print(const arg_t&... args);

template<typename... arg_t>
std::ostream& print_error(const arg_t&... args);

#include <core_library/print.inl>

#endif // CORELIBRARY_PRINT_HPP_
