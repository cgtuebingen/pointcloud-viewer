#include <core_library/print.hpp>
#include <core_library/color_palette.hpp>
#include <geometry/swizzle.hpp>
#include <geometry/transform.hpp>
#include <geometry/perpendicular.hpp>
#include <renderer/gl450/debug/debug_mesh.hpp>

#include <glm/gtc/constants.hpp>

const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION = 0;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1 = 1;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR = 2;
const int DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2 = 3;

const int DEBUG_MESH_VERTEX_BUFFER_BINDING = 0;

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

DebugMesh DebugMesh::axis(glm::bvec3 axis, float length, float tip_length)
{
  Generator generator;

  const int brightness = 1;
  const glm::vec3 colors[] = {color_palette::red[brightness],
                             color_palette::green[brightness],
                             color_palette::blue[brightness]};

  for(int dim=0; dim<3; ++dim)
  {
    if(axis[dim])
    {
      glm::vec3 target(0);

      target[dim] = length;

      generator.next_attribute.color = colors[dim];
      generator.add_arrow(glm::vec3(0), target, tip_length);
    }
  }

  return generator.to_mesh();
}

DebugMesh DebugMesh::grid(int repetition_per_side, float cell_size, glm::vec3 color, glm::vec3 origin, glm::vec3 axis_1, glm::vec3 axis_2)
{
  Generator generator;
  generator.next_attribute.color = color;

  glm::vec3 half_axis_1 = axis_1 * (cell_size * repetition_per_side);
  glm::vec3 half_axis_2 = axis_2 * (cell_size * repetition_per_side);

  for(int i=-repetition_per_side; i<=repetition_per_side; ++i)
  {
    const float offset = cell_size * i;

    generator.add_vertex(origin+axis_1*offset+half_axis_2);
    generator.add_vertex(origin+axis_1*offset-half_axis_2);
    generator.add_vertex(origin+axis_2*offset+half_axis_1);
    generator.add_vertex(origin+axis_2*offset-half_axis_1);
  }

  return generator.to_mesh();
}


// ======== Renderer ============================================================

typedef gl::VertexArrayObject::Attribute Attribute;

DebugMeshRenderer::DebugMeshRenderer()
  : shader_object("debug_mesh_renderer"),
    vertex_array_object({
                        Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_BUFFER_BINDING),
                        Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_BUFFER_BINDING),
                        Attribute(Attribute::Type::FLOAT, 3, DEBUG_MESH_VERTEX_BUFFER_BINDING),
                        Attribute(Attribute::Type::FLOAT, 1, DEBUG_MESH_VERTEX_BUFFER_BINDING)})
{
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                    format("#version 450 core\n"
                                           "\n"
                                           "layout(location = ", DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION, ")\n",
                                           "in vec3 vertex_position;\n"
                                           "layout(location = ", DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR, ")\n",
                                           "in vec3 vertex_color;\n"
                                           "\n"
                                           "out vec3 color;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  gl_Position = vec4(vertex_position.xyz, 1);\n"
                                           "  color = vertex_color;\n"
                                           "}\n"),
                                    "PointRenderer::Implementation::Implementation() // vertex");
  shader_object.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                                    format("#version 450 core\n"
                                           "\n"
                                           "in vec3 color;\n"
                                           "\n"
                                           "layout(location=0)\n"
                                           "out vec4 fragment_color;\n"
                                           "\n"
                                           "void main()\n"
                                           "{\n"
                                           "  fragment_color = vec4(color, 1);\n"
                                           "}\n"),
                                    "PointRenderer::Implementation::Implementation() // fragment");
  shader_object.CreateProgram();
}

void DebugMeshRenderer::begin()
{
  vertex_array_object.Bind();
  shader_object.Activate();
}

void DebugMeshRenderer::render(const DebugMesh& mesh)
{
  mesh.vertex_buffer.BindVertexBuffer(DEBUG_MESH_VERTEX_BUFFER_BINDING, 0, GLsizei(vertex_array_object.GetVertexStride(DEBUG_MESH_VERTEX_BUFFER_BINDING)));
  GL_CALL(glDrawArrays, GL_LINES, 0, mesh.num_vertices);
}

void DebugMeshRenderer::end()
{
  shader_object.Deactivate();
}

// ======== Painter ============================================================


DebugMesh::Generator::Generator()
{
  transformations.push(glm::mat4(1));
}


void DebugMesh::Generator::begin_strip(strip_t close)
{
  this->strip_index = 0;
  if(close == CLOSE)
    this->first_strip_vertex = vertices.length();
  else
    this->first_strip_vertex = std::numeric_limits<int>::max();
}

void DebugMesh::Generator::end_strip()
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


void DebugMesh::Generator::add_vertex(const glm::vec3& position)
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
  vertices.last().color = next_attribute.color;
  vertices.last().parameter1 = next_attribute.parameter1;
  vertices.last().parameter2 = next_attribute.parameter2;

  vertices.last().position = transform_point(transformations.top(), vertices.last().position);
}

void DebugMesh::Generator::add_vertex(const glm::vec2& position, float z)
{
  add_vertex(glm::vec3(position, z));
}

void DebugMesh::Generator::add_vertex(float x, float y, float z)
{
  add_vertex(glm::vec3(x, y, z));
}


void DebugMesh::Generator::add_circle(float radius, int nPoints)
{
  begin_strip(CLOSE);
  for(int i=0; i<nPoints; ++i)
  {
    float angle = i * glm::two_pi<float>() / nPoints;

    add_vertex(glm::vec2(glm::cos(angle), glm::sin(angle)) * radius);
  }
  end_strip();
}


void DebugMesh::Generator::add_sphere(float radius, int nPoints)
{
  add_circle(radius, nPoints);

  push_matrix(glm::vec3(0), glm::vec3(1, 0, 0));
  add_circle(radius, nPoints);
  pop_matrix();

  push_matrix(glm::vec3(0), glm::vec3(0, 1, 0));
  add_circle(radius, nPoints);
  pop_matrix();
}


void DebugMesh::Generator::add_cylinder(float radius, float length, int nPoints)
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


void DebugMesh::Generator::add_rect(const glm::vec2& min, const glm::vec2& max)
{
  begin_strip(CLOSE);
  add_vertex(min.x, min.y);
  add_vertex(min.x, max.y);
  add_vertex(max.x, max.y);
  add_vertex(max.x, min.y);
  end_strip();
}


void DebugMesh::Generator::add_cube(const glm::vec3& min, const glm::vec3& max)
{
  push_matrix(glm::vec3(0, 0, min.z));
  add_rect(xy(min), xy(max));
  pop_matrix();

  push_matrix(glm::vec3(0, 0, max.z));
  add_rect(xy(min), xy(max));
  pop_matrix();

  for(const glm::vec2& corner_id : {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, 1)})
  {
    glm::vec2 corner = corner_id*xy(max) + (1.f-corner_id)*xy(min);

    add_vertex(xy(corner), min.z);
    add_vertex(xy(corner), max.z);
  }
}


void DebugMesh::Generator::add_arrow(float length, float tip_length)
{
  add_vertex(0, 0, 0);
  add_vertex(0, 0, length);

  for(const glm::vec2& dir : {glm::vec2(-1, -1), glm::vec2(-1, 1), glm::vec2(1, -1), glm::vec2(1, 1)})
  {
    add_vertex(0, 0, length);
    add_vertex(dir*tip_length, length-tip_length);
  }
}

void DebugMesh::Generator::add_arrow(glm::vec3 origin, glm::vec3 tip, float tip_length)
{
  push_matrix(origin, glm::normalize(tip-origin));

  float arrow_length = glm::length(tip-origin);
  add_arrow(arrow_length, tip_length);

  pop_matrix();
}

void DebugMesh::Generator::push_matrix(const glm::vec3& position, bool multiply)
{
  glm::mat4 matrix = glm::mat4(1);
  matrix[3] = glm::vec4(position, 1);

  push_matrix(matrix, multiply);
}

void DebugMesh::Generator::push_matrix(const glm::vec3& position, const glm::vec3& normal, bool multiply)
{
  push_matrix(position, normal, find_best_perpendicular(normal), multiply);
}

void DebugMesh::Generator::push_matrix(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& firstPointDirection, bool multiply)
{
  glm::mat4 matrix = glm::mat4(glm::vec4(firstPointDirection, 0),
                               glm::vec4(glm::cross(normal, firstPointDirection), 0),
                               glm::vec4(normal, 0),
                               glm::vec4(position, 1));

  push_matrix(matrix, multiply);
}

void DebugMesh::Generator::push_matrix(const glm::mat4& matrix, bool multiply)
{
  Q_ASSERT(transformations.size() > 0);

  if(multiply)
    transformations.push(transformations.top() * matrix);
  else
    transformations.push(matrix);
}

void DebugMesh::Generator::pop_matrix()
{
  Q_ASSERT(transformations.size() > 0);
  transformations.pop();

  if(transformations.size() == 0)
    transformations.push(glm::mat4(1));
}


DebugMesh DebugMesh::Generator::to_mesh() const
{
  return DebugMesh(vertices.data(), vertices.length());
}


} // namespace gl450
} // namespace renderer


