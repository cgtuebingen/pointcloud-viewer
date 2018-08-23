set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.5 REQUIRED)

add_library(core_library STATIC
  color_palette.cpp
  color_palette.hpp
  image.cpp
  image.hpp
  padding.hpp
  print.hpp
  print.inl
  stack.hpp
  stack.inl
  types.hpp
)

target_link_libraries(core_library PUBLIC Qt5::Gui glm)
target_compile_options(core_library PUBLIC  "-Werror=return-type")
