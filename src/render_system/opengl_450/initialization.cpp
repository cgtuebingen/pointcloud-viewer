#include <glad/glad.h>

#include <render_system/initialization.hpp>

namespace render_system {

void initialize_gl(gl_proc_loader_t loader)
{
  // initialize glad to enable using OpenGL 4.5 functions
  gladLoadGLLoader(loader);
}

} //namespace render_system
