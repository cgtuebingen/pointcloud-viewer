// GLHelper is meant as "copy and paste"  helper class collection. As such you are encouraged to integrate it seamlessly.
// This files contains several type and function definitions that you usually want to alter for a better integration into your application.

#include <string>
#include <cassert>
#include <iostream>
#include <limits>  // for std::numeric_limits in tetxureview.hpp
#include <string.h> // for memcmp in samplerobject.h
#include <vector> // for std::vector in shaderobject.cpp
#include <algorithm> // for std::count in shaderobject.cpp
#include <glm/glm.hpp>

// General settings.

// If activated, Texture2D has a FromFile method which uses stbi to load images and create mipmaps.
//#define TEXTURE2D_FROMFILE_STBI

// Activates output of shader compile logs to log.
#define SHADER_COMPILE_LOGS



// Assert
#ifdef _DEBUG
#define GLHELPER_ASSERT(condition, message) do { \
	if(!(condition)) std::cerr << message; \
	assert(condition); } while(false)
#else
#define GLHELPER_ASSERT(condition, string) do { } while(false)
#endif



// Logging
#define GLHELPER_LOG_ERROR(message)		do { std::cerr << "Error: " << message << " (" << __FILE__ << ", (" << __LINE__ << ")" << std::endl; } while(false)
#define GLHELPER_LOG_WARNING(message)	do { std::cerr << "Warning: " << message << " (" << __FILE__ << ", (" << __LINE__ << ")" << std::endl; } while(false)

#if 0
#define GLHELPER_LOG_INFO(message)		do { std::clog << "Info: " << message << " (" << __FILE__ << ", (" << __LINE__ << ")" << std::endl; } while(false)
#else
#define GLHELPER_LOG_INFO(message)		do { } while(false)
#endif



// Vector & Matrix types.
#include <cstdint>
namespace gl
{
	namespace Details
	{
    const unsigned int s_numUBOBindings = 64;	/// Arbitrary value based on observation: http://delphigl.de/glcapsviewer/gl_stats_caps_single.php?listreportsbycap=GL_MAX_COMBINED_UNIFORM_BLOCKS
    const unsigned int s_numSSBOBindings = 16; /// Arbitrary value, based on observation: http://delphigl.de/glcapsviewer/gl_stats_caps_single.php?listreportsbycap=GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
    const unsigned int s_numAtomicCounterBindings = 8; /// Arbitrary value, based on observation: http://delphigl.de/glcapsviewer/gl_stats_caps_single.php?listreportsbycap=GL_MAX_COMBINED_ATOMIC_COUNTERS
	}

	typedef glm::vec2 Vec2;
	typedef glm::vec3 Vec3;
	typedef glm::vec4 Vec4;

	typedef glm::ivec2 IVec2;
	typedef glm::ivec3 IVec3;
  typedef glm::ivec4 IVec4;

  typedef glm::uvec2 UVec2;
  typedef glm::uvec3 UVec3;
  typedef glm::uvec4 UVec4;

  typedef glm::mat3 Mat3;
  typedef glm::mat4 Mat4;
};

// A std::vector of all include paths shaders will be looked for, if an #include<...> statement was found during parsing an glsl script
#define SHADER_EXPAND_GLOBAL_INCLUDE(x) ""

// OpenGL header.

#include <glad/glad.h>

#define GLAPIENTRY

