#define GLM_FORCE_SWIZZLE
#include <core_library/geometry.hpp>
#include <renderer/gl450/debug/debug_mesh.hpp>

#include <glm/gtc/constants.hpp>

const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION = 0;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1 = 1;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR = 2;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2 = 3;

namespace renderer {
namespace gl450 {


DebugMesh::DebugMesh(const Vertex* vertices, int numVertices)
  : vertexBuffer(int(sizeof(Vertex))*numVertices, gl::Buffer::UsageFlag::IMMUTABLE, vertices),
    numVertices(numVertices)
{
}

DebugMesh::~DebugMesh()
{
}


DebugMesh::DebugMesh(DebugMesh&& debugMesh)
  : vertexBuffer(std::move(debugMesh.vertexBuffer)),
    numVertices(debugMesh.numVertices)
{
  debugMesh.numVertices = 0;
}


gl::VertexArrayObject DebugMesh::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  return gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION),
                                Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1),
                                Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR),
                                Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2)});
}


void DebugMesh::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  const int vertexBufferBinding = 0;
  vertexBuffer.BindVertexBuffer(vertexBufferBinding, 0, GLsizei(vertexArrayObject.GetVertexStride(vertexBufferBinding)));
}


void DebugMesh::draw()
{
  GL_CALL(glDrawArrays, GL_LINES, 0, numVertices);
}


// ======== Painter ============================================================


DebugMesh::Painter::Painter()
{
  transformations.push(glm::mat4(1));
}


void DebugMesh::Painter::beginStrip(bool close)
{
  this->stripIndex = 0;
  if(close)
    this->firstStripVertex = vertices.length();
  else
    this->firstStripVertex = std::numeric_limits<int>::max();
}

void DebugMesh::Painter::endStrip()
{
  if(this->firstStripVertex < vertices.length())
  {
    pushMatrix(glm::mat4(1), false);

    // duplicating, because passing a reference to a value within the array vertices, which might be modified with vertices.resize within addVertex is dangerous
    glm::vec3 startPosition = vertices[this->firstStripVertex].position;

    addVertex(startPosition);
    popMatrix();
  }

  this->stripIndex = -1;
}


void DebugMesh::Painter::addVertex(const glm::vec3& position)
{
  Q_ASSERT(transformations.size() > 0);

  if(stripIndex >= 0)
  {
    if(vertices.length() > 0 && stripIndex > 1)
    {
      Vertex v = vertices.last();
      vertices.append(v);
    }
    stripIndex  = (stripIndex+1);
  }

  vertices.resize(vertices.length()+1);

  vertices.last().position = position;
  vertices.last().color = nextAttribute.color;
  vertices.last().parameter1 = nextAttribute.parameter1;
  vertices.last().parameter2 = nextAttribute.parameter2;

  vertices.last().position = transform_point(transformations.top(), vertices.last().position);
}

void DebugMesh::Painter::addVertex(const glm::vec2& position, float z)
{
  addVertex(glm::vec3(position, z));
}

void DebugMesh::Painter::addVertex(float x, float y, float z)
{
  addVertex(glm::vec3(x, y, z));
}


void DebugMesh::Painter::addCircle(float radius, int nPoints)
{
  beginStrip(true);
  for(int i=0; i<nPoints; ++i)
  {
    float angle = i * glm::two_pi<float>() / nPoints;

    addVertex(glm::vec2(glm::cos(angle), glm::sin(angle)) * radius);
  }
  endStrip();
}


void DebugMesh::Painter::addSphere(float radius, int nPoints)
{
  addCircle(radius, nPoints);

  pushMatrix(glm::vec3(0), glm::vec3(1, 0, 0));
  addCircle(radius, nPoints);
  popMatrix();

  pushMatrix(glm::vec3(0), glm::vec3(0, 1, 0));
  addCircle(radius, nPoints);
  popMatrix();
}


void DebugMesh::Painter::addCylinder(float radius, float length, int nPoints)
{
  pushMatrix(glm::vec3(0, 0, 0.5f)*length, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
  addCircle(radius, nPoints);
  popMatrix();

  pushMatrix(-glm::vec3(0, 0, 0.5f)*length, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
  addCircle(radius, nPoints);
  popMatrix();

  for(const glm::vec3& side : {glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0), glm::vec3(0, 1, 0)})
  {
    addVertex(side*radius + glm::vec3(0, 0, .5f)*length);
    addVertex(side*radius + glm::vec3(0, 0,-.5f)*length);
  }
}


void DebugMesh::Painter::addRect(const glm::vec2& min, const glm::vec2& max)
{
  beginStrip(true);
  addVertex(min.x, min.y);
  addVertex(min.x, max.y);
  addVertex(max.x, max.y);
  addVertex(max.x, min.y);
  endStrip();
}


void DebugMesh::Painter::addCube(const glm::vec3& min, const glm::vec3& max)
{
  pushMatrix(glm::vec3(0, 0, min.z));
  addRect(min.xy(), max.xy());
  popMatrix();

  pushMatrix(glm::vec3(0, 0, max.z));
  addRect(min.xy(), max.xy());
  popMatrix();

  for(const glm::vec2& corner_id : {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)})
  {
    glm::vec2 corner = corner_id*max.xy() + (1.f-corner_id)*min.xy();

    addVertex(corner.xy(), min.z);
    addVertex(corner.xy(), max.z);
  }
}


void DebugMesh::Painter::addArrow(float length, float tipLength)
{
  addVertex(0, 0, 0);
  addVertex(0, 0, length);

  for(const glm::vec2& dir : {glm::vec2(-1, -1), glm::vec2(-1, 1), glm::vec2(1, -1), glm::vec2(1, 1)})
  {
    addVertex(0, 0, length);
    addVertex(dir*tipLength, length-tipLength);
  }
}

void DebugMesh::Painter::pushMatrix(const glm::vec3& position, bool multiply)
{
  glm::mat4 matrix = glm::mat4(1);
  matrix[3] = glm::vec4(position, 1);

  pushMatrix(matrix, multiply);
}

void DebugMesh::Painter::pushMatrix(const glm::vec3& position, const glm::vec3& normal, bool multiply)
{
  pushMatrix(position, normal, find_best_perpendicular(normal), multiply);
}

void DebugMesh::Painter::pushMatrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply)
{
  glm::mat4 matrix = glm::mat4(glm::vec4(firstPointDirection, 0),
                               glm::vec4(glm::cross(normal, firstPointDirection), 0),
                               glm::vec4(normal, 0),
                               glm::vec4(position, 1));

  pushMatrix(matrix, multiply);
}

void DebugMesh::Painter::pushMatrix(const glm::mat4& matrix, bool multiply)
{
  Q_ASSERT(transformations.size() > 0);

  if(multiply)
    transformations.push(transformations.top() * matrix);
  else
    transformations.push(matrix);
}

void DebugMesh::Painter::popMatrix()
{
  Q_ASSERT(transformations.size() > 0);
  transformations.pop();

  if(transformations.size() == 0)
    transformations.push(glm::mat4(1));
}


DebugMesh DebugMesh::Painter::toMesh() const
{
  return DebugMesh(vertices.data(), vertices.length());
}


} // namespace gl450
} // namespace renderer


