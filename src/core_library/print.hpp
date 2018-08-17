#ifndef CORELIBRARY_PRINT_HPP_
#define CORELIBRARY_PRINT_HPP_

#include <string>
#include <sstream>

/*
Helper functions for operating with strings.

`format` helps formatting a string. For example

    format("answer to the life, universe and everything: ", 42, ". Deep inside, you know it's true!")

`print` do the same, but they already prints the result to the console.
`print` do the same, but includes an std::endl at the end

`PRINT` is a macro printing an expression along with the expression itself. For example PRINT(6*7) prints `6*7: 42`
*/

template<typename... arg_t>
std::string format(const arg_t&... args);

template<typename... arg_t>
std::ostream& println(const arg_t&... args);

template<typename... arg_t>
std::ostream& print(const arg_t&... args);

template<typename... arg_t>
std::ostream& println_error(const arg_t&... args);

template<typename... arg_t>
std::ostream& print_error(const arg_t&... args);

#define PRINT(expression) println(#expression ": ", expression)

#include <core_library/print.inl>

#endif // CORELIBRARY_PRINT_HPP_
