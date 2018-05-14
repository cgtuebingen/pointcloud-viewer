add_library(opengl_450 STATIC
     initialization.cpp
     point_renderer.cpp
     ../initialization.hpp
     ../point_renderer.hpp
)

target_link_libraries(opengl_450 PUBLIC core_library glad glm)
