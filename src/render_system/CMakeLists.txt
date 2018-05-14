add_subdirectory(opengl_450)

set(USED_RENDER_SYSTEM opengl_450)

add_library(render_system INTERFACE)
target_link_libraries(render_system INTERFACE ${USED_RENDER_SYSTEM})
