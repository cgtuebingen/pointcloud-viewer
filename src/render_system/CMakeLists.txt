add_subdirectory(opengl_450)

set(USED_RENDER_SYSTEM opengl_450)

add_library(render_system STATIC
  initialization.hpp
  point_renderer.cpp
  point_renderer.hpp
)
target_link_libraries(render_system PUBLIC ${USED_RENDER_SYSTEM})
