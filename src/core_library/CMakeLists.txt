find_package(Qt5Widgets 5.5 REQUIRED)

add_library(core_library STATIC
  geometry.hpp
  geometry.inl
  print.cpp
  print.hpp
  print.inl
)

target_link_libraries(core_library PUBLIC Qt5::Core glm)
