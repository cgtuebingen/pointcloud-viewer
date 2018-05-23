find_package(Qt5Widgets 5.5 REQUIRED)

add_library(geometry STATIC
  dummy.cpp
  perpendicular.hpp
  perpendicular.inl
  swizzle.hpp
  swizzle.inl
  transform.hpp
  transform.inl
)

target_link_libraries(geometry PUBLIC core_library glm)
