find_package(Qt5Widgets 5.5 REQUIRED)

add_library(geometry STATIC
  dummy.cpp
  geometry.hpp
  geometry.inl
)

target_link_libraries(geometry PUBLIC core_library glm)
