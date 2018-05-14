#ifndef RENDERSYSTEM_INITIALIZATION_HPP_
#define RENDERSYSTEM_INITIALIZATION_HPP_

namespace render_system {

typedef void* (* gl_proc_loader_t)(const char *name);

void initialize_gl(gl_proc_loader_t loader);

} //namespace render_system

#endif // RENDERSYSTEM_INITIALIZATION_HPP_
