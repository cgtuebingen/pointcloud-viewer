add_library(gl450 STATIC
 debug/debug_mesh.cpp
 debug/debug_mesh.hpp
 declarations.hpp
 locate_shaders.cpp
 locate_shaders.hpp
 point_renderer.cpp
 point_renderer.hpp
 uniforms.cpp
 uniforms.hpp
)

target_link_libraries(gl450 PUBLIC core_library glad glm glhelper pointcloud)
