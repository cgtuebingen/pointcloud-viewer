find_package(Qt5Widgets 5.5 REQUIRED)

add_library(geometry STATIC
  aabb.cpp
  aabb.hpp
  cone.inl
  cone.hpp
  frame.cpp
  frame.hpp
  frame.inl
  perpendicular.hpp
  perpendicular.inl
  ray.hpp
  ray.inl
  swizzle.hpp
  swizzle.inl
  transform.hpp
  transform.inl
)

target_link_libraries(geometry PUBLIC core_library glm)
