add_library(gl450 STATIC
 debug/debug_mesh.cpp
 debug/debug_mesh.hpp
 point_renderer.cpp
 point_renderer.hpp
 declarations.hpp
)

target_link_libraries(gl450 PUBLIC core_library glad glm glhelper)
