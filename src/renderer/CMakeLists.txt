add_subdirectory(gl450)

set(USED_RENDER_SYSTEM gl450)

add_library(renderer INTERFACE)
target_link_libraries(renderer INTERFACE ${USED_RENDER_SYSTEM})
