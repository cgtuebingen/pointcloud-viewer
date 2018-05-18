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


DebugMesh::DebugMesh(const vertex_t* vertices, int num_vertices)
  : vertex_buffer(int(sizeof(vertex_t))*num_vertices, gl::Buffer::UsageFlag::IMMUTABLE, vertices),
    num_vertices(num_vertices)
{
}

DebugMesh::~DebugMesh()
{
}


DebugMesh::DebugMesh(DebugMesh&& debugMesh)
  : vertex_buffer(std::move(debugMesh.vertex_buffer)),
    num_vertices(debugMesh.num_vertices)
{
  debugMesh.num_vertices = 0;
}


gl::VertexArrayObject DebugMesh::generateVertexArrayObject()
{
  typedef gl::VertexArrayObject::Attribute Attribute;

  return gl::VertexArrayObject({Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION),
                                Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1),
                                Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR),
                                Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2)});
}

// TODO binding and drawing should be done by a debug mesh renderer owning the shader and vertex array object
void DebugMesh::bind(const gl::VertexArrayObject& vertexArrayObject)
{
  const int vertexBufferBinding = 0;
  vertex_buffer.BindVertexBuffer(vertexBufferBinding, 0, GLsizei(vertexArrayObject.GetVertexStride(vertexBufferBinding)));
}


void DebugMesh::draw()
{
  GL_CALL(glDrawArrays, GL_LINES, 0, num_vertices);
}


// ======== Painter ============================================================


DebugMesh::Painter::Painter()
{
  transformations.push(glm::mat4(1));
}


void DebugMesh::Painter::begin_strip(bool close)
{
  this->strip_index = 0;
  if(close)
    this->first_strip_vertex = vertices.length();
  else
    this->first_strip_vertex = std::numeric_limits<int>::max();
}

void DebugMesh::Painter::end_strip()
{
  if(this->first_strip_vertex < vertices.length())
  {
    push_matrix(glm::mat4(1), false);

    // duplicating, because passing a reference to a value within the array vertices, which might be modified with vertices.resize within addVertex is dangerous
    glm::vec3 startPosition = vertices[this->first_strip_vertex].position;

    add_vertex(startPosition);
    pop_matrix();
  }

  this->strip_index = -1;
}


void DebugMesh::Painter::add_vertex(const glm::vec3& position)
{
  Q_ASSERT(transformations.size() > 0);

  if(strip_index >= 0)
  {
    if(vertices.length() > 0 && strip_index > 1)
    {
      vertex_t v = vertices.last();
      vertices.append(v);
    }
    strip_index  = (strip_index+1);
  }

  vertices.resize(vertices.length()+1);

  vertices.last().position = position;
  vertices.last().color = nextAttribute.color;
  vertices.last().parameter1 = nextAttribute.parameter1;
  vertices.last().parameter2 = nextAttribute.parameter2;

  vertices.last().position = transform_point(transformations.top(), vertices.last().position);
}

void DebugMesh::Painter::add_vertex(const glm::vec2& position, float z)
{
  add_vertex(glm::vec3(position, z));
}

void DebugMesh::Painter::add_vertex(float x, float y, float z)
{
  add_vertex(glm::vec3(x, y, z));
}


void DebugMesh::Painter::add_circle(float radius, int nPoints)
{
  begin_strip(true);
  for(int i=0; i<nPoints; ++i)
  {
    float angle = i * glm::two_pi<float>() / nPoints;

    add_vertex(glm::vec2(glm::cos(angle), glm::sin(angle)) * radius);
  }
  end_strip();
}


void DebugMesh::Painter::add_sphere(float radius, int nPoints)
{
  add_circle(radius, nPoints);

  push_matrix(glm::vec3(0), glm::vec3(1, 0, 0));
  add_circle(radius, nPoints);
  pop_matrix();

  push_matrix(glm::vec3(0), glm::vec3(0, 1, 0));
  add_circle(radius, nPoints);
  pop_matrix();
}


void DebugMesh::Painter::add_cylinder(float radius, float length, int nPoints)
{
  push_matrix(glm::vec3(0, 0, 0.5f)*length, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
  add_circle(radius, nPoints);
  pop_matrix();

  push_matrix(-glm::vec3(0, 0, 0.5f)*length, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0));
  add_circle(radius, nPoints);
  pop_matrix();

  for(const glm::vec3& side : {glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0), glm::vec3(0, 1, 0)})
  {
    add_vertex(side*radius + glm::vec3(0, 0, .5f)*length);
    add_vertex(side*radius + glm::vec3(0, 0,-.5f)*length);
  }
}


void DebugMesh::Painter::add_rect(const glm::vec2& min, const glm::vec2& max)
{
  begin_strip(true);
  add_vertex(min.x, min.y);
  add_vertex(min.x, max.y);
  add_vertex(max.x, max.y);
  add_vertex(max.x, min.y);
  end_strip();
}


void DebugMesh::Painter::add_cube(const glm::vec3& min, const glm::vec3& max)
{
  push_matrix(glm::vec3(0, 0, min.z));
  add_rect(min.xy(), max.xy());
  pop_matrix();

  push_matrix(glm::vec3(0, 0, max.z));
  add_rect(min.xy(), max.xy());
  pop_matrix();

  for(const glm::vec2& corner_id : {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)})
  {
    glm::vec2 corner = corner_id*max.xy() + (1.f-corner_id)*min.xy();

    add_vertex(corner.xy(), min.z);
    add_vertex(corner.xy(), max.z);
  }
}


void DebugMesh::Painter::add_arrow(float length, float tipLength)
{
  add_vertex(0, 0, 0);
  add_vertex(0, 0, length);

  for(const glm::vec2& dir : {glm::vec2(-1, -1), glm::vec2(-1, 1), glm::vec2(1, -1), glm::vec2(1, 1)})
  {
    add_vertex(0, 0, length);
    add_vertex(dir*tipLength, length-tipLength);
  }
}

void DebugMesh::Painter::push_matrix(const glm::vec3& position, bool multiply)
{
  glm::mat4 matrix = glm::mat4(1);
  matrix[3] = glm::vec4(position, 1);

  push_matrix(matrix, multiply);
}

void DebugMesh::Painter::push_matrix(const glm::vec3& position, const glm::vec3& normal, bool multiply)
{
  push_matrix(position, normal, find_best_perpendicular(normal), multiply);
}

void DebugMesh::Painter::push_matrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply)
{
  glm::mat4 matrix = glm::mat4(glm::vec4(firstPointDirection, 0),
                               glm::vec4(glm::cross(normal, firstPointDirection), 0),
                               glm::vec4(normal, 0),
                               glm::vec4(position, 1));

  push_matrix(matrix, multiply);
}

void DebugMesh::Painter::push_matrix(const glm::mat4& matrix, bool multiply)
{
  Q_ASSERT(transformations.size() > 0);

  if(multiply)
    transformations.push(transformations.top() * matrix);
  else
    transformations.push(matrix);
}

void DebugMesh::Painter::pop_matrix()
{
  Q_ASSERT(transformations.size() > 0);
  transformations.pop();

  if(transformations.size() == 0)
    transformations.push(glm::mat4(1));
}


DebugMesh DebugMesh::Painter::to_mesh() const
{
  return DebugMesh(vertices.data(), vertices.length());
}


} // namespace gl450
} // namespace renderer


