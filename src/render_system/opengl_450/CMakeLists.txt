add_library(opengl_450 STATIC
     point_renderer.cpp
)

target_link_libraries(opengl_450 PUBLIC core_library glad glm glhelper)
