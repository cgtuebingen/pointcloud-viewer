#ifndef RENDERSYSTEM_GL450_DEBUG_DEBUGMESH_H
#define RENDERSYSTEM_GL450_DEBUG_DEBUGMESH_H

#include <renderer/gl450/declarations.hpp>
#include <geometry/frame.hpp>
#include <geometry/aabb.hpp>
#include <geometry/cone.hpp>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>

#include <QStack>

namespace renderer {
namespace gl450 {

/**
A class responsible for rendering small visualizations (grids, axis, arrows, the camera)
*/
class DebugMesh final
{
public:
  friend class DebugMeshRenderer;
  struct vertex_t;
  class Generator;

  DebugMesh();
  DebugMesh(const vertex_t* vertices, int num_vertices);
  ~DebugMesh();

  DebugMesh(DebugMesh&& debugMesh);
  DebugMesh& operator=(DebugMesh&&);

  DebugMesh(const DebugMesh&) = delete;
  DebugMesh& operator=(const DebugMesh&) = delete;

  static DebugMesh aabb(aabb_t aabb, glm::vec3 color);
  static DebugMesh turntable_point(glm::vec3 origin, float r=1.f, const glm::vec3 color=glm::vec3(1,0.5,1));
  static DebugMesh axis(glm::bvec3 axis = glm::bvec3(true), float length=1.f, float tip_length=0.1f);
  static DebugMesh grid(int repetition_per_side, float cell_size, glm::vec3 color, glm::vec3 origin=glm::vec3(0), glm::vec3 axis_1 = glm::vec3(1,0,0), glm::vec3 axis_2 = glm::vec3(0,1,0));
  static DebugMesh path(int path_length, std::function<frame_t(int)> frame_for_index, int selection);
  static DebugMesh cone(cone_t cone);
  static DebugMesh highlighted_point(glm::vec3 coordinate, glm::vec3 color, float radius);

private:
  gl::Buffer vertex_buffer;
  int num_vertices;
};


struct DebugMesh::vertex_t final
{
  glm::vec3 position;
  float parameter1;
  glm::vec3 color;
  float parameter2;
};


class DebugMeshRenderer final
{
public:
  DebugMeshRenderer();

  DebugMeshRenderer(DebugMeshRenderer&& point_renderer);
  DebugMeshRenderer& operator=(DebugMeshRenderer&& point_renderer);

  void begin();
  void render(const DebugMesh& mesh);
  void end();

private:
  gl::ShaderObject shader_object;
  gl::VertexArrayObject vertex_array_object;
};


class DebugMesh::Generator final
{
public:
  enum strip_t
  {
    CLOSE,
    OPEN,
  };

  struct current_attribute_t
  {
    glm::vec3 color = glm::vec3(1, 0.5, 0);
    float parameter1 = 0.f;
    float parameter2 = 0.f;
  };

  Generator();

  current_attribute_t next_attribute;

  QVector<vertex_t> vertices;

  void begin_strip(strip_t close);
  void end_strip();

  void add_axis(glm::bvec3 axis = glm::bvec3(true), float length=1.f, float tip_length=0.1f, bool rgb=true);

  void add_vertex(const glm::vec3& position);
  void add_vertex(const glm::vec2& position, float z=0);
  void add_vertex(float x, float y, float z=0);

  void add_circle(float radius, int nPoints);
  void add_sphere(float radius, int nPoints);
  void add_cylinder(float radius, float length, int nPoints);
  void add_rect(const glm::vec2& min, const glm::vec2& max);
  void add_cube(const glm::vec3& min, const glm::vec3& max);
  void add_arrow(float length, float tip_length);
  void add_arrow(glm::vec3 origin, glm::vec3 tip, float tip_length);

  void push_matrix(const glm::vec3& position, bool multiply = true);
  void push_matrix(const glm::vec3& position, const glm::vec3& normal, bool multiply = true);
  void push_matrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply = true);
  void push_matrix(const glm::mat4& matrix=glm::mat4(1), bool multiply = true);
  void pop_matrix();

  DebugMesh to_mesh() const;

private:
  QStack<glm::mat4> transformations;
  int strip_index = -1;
  int first_strip_vertex;
};


} // namespace gl450
} // namespace renderer

#endif // RENDERSYSTEM_GL450_DEBUG_DEBUGMESH_H

