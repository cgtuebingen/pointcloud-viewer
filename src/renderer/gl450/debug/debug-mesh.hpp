#ifndef GLRT_RENDERER_DEBUGGING_DEBUGMESH_H
#define GLRT_RENDERER_DEBUGGING_DEBUGMESH_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>

#include <QStack>

namespace glrt {
namespace renderer {
namespace debugging {


class DebugMesh final
{
public:
  struct Vertex;
  class Painter;

  DebugMesh(const Vertex* vertices, int numVertices);
  ~DebugMesh();
  DebugMesh(DebugMesh&&debugMesh);


  DebugMesh& operator=(DebugMesh&&) = delete;
  DebugMesh(const DebugMesh&) = delete;
  DebugMesh& operator=(const DebugMesh&) = delete;

  static gl::VertexArrayObject generateVertexArrayObject();

  void bind(const gl::VertexArrayObject& vertexArrayObject);
  void draw();

private:
  gl::Buffer vertexBuffer;
  int numVertices;
};


struct DebugMesh::Vertex final
{
  glm::vec3 position;
  float parameter1;
  glm::vec3 color;
  float parameter2;
};


class DebugMesh::Painter final
{
public:
  struct CurrentAttribute
  {
    glm::vec3 color = glm::vec3(1, 0.5, 0);
    float parameter1 = 0.f;
    float parameter2 = 0.f;
  }nextAttribute;

  Painter();

  QVector<DebugMesh::Vertex> vertices;

  void beginStrip(bool close);
  void endStrip();

  void addVertex(const glm::vec3& position);
  void addVertex(const glm::vec2& position, float z=0);
  void addVertex(float x, float y, float z=0);

  void addCircle(float radius, int nPoints);
  void addSphere(float radius, int nPoints);
  void addCylinder(float radius, float length, int nPoints);
  void addRect(const glm::vec2& min, const glm::vec2& max);
  void addCube(const glm::vec3& min, const glm::vec3& max);
  void addArrow(float length, float tipLength);

  void pushMatrix(const glm::vec3& position, bool multiply = true);
  void pushMatrix(const glm::vec3& position, const glm::vec3& normal, bool multiply = true);
  void pushMatrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply = true);
  void pushMatrix(const glm::mat4& matrix=glm::mat4(1), bool multiply = true);
  void popMatrix();

  DebugMesh toMesh() const;

private:
  QStack<glm::mat4> transformations;
  int stripIndex = -1;
  int firstStripVertex;
};


} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_DEBUGMESH_H

