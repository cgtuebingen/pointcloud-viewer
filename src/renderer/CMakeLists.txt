add_subdirectory(gl450)

set(USED_RENDER_SYSTEM gl450)

add_library(render_system INTERFACE)
target_link_libraries(render_system INTERFACE ${USED_RENDER_SYSTEM})
