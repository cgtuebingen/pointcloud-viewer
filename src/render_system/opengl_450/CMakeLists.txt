add_library(opengl_450 STATIC initialization.cpp ../initialization.hpp)

target_link_libraries(opengl_450 PUBLIC glad)
