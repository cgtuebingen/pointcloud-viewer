#include <string>
#include <sstream>

/*
Helper functions for operating with strings.

`format` helps formatting a string. For example

    format("answer to the life, universe and everything: ", 42, ". Deep inside, you know it's true!")
*/

template<typename... arg_t>
std::string format(const arg_t&... args);

/*
Modifiers to change the formating of numbers.

Usage:

    format("This is the item #", format_width_t(3), item.get_index())
*/

enum class format_precision_t : streamsize
{
  DEFAULT = 8,
};

enum class format_width_t : streamsize
{
  DEFAULT = 0,
};


#include <core_library/string.impl>
