#ifdef USE_ASSIMP

#include <pointcloud/importer/assimp_importer.hpp>
#include <core_library/print.hpp>
#include <core_library/types.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtx/io.hpp>

#include <QThread>
#include <QAbstractEventDispatcher>

#include <iostream>

// see http://assimp.sourceforge.net/lib_html/usage.html

AssimpImporter::AssimpImporter(const std::string& input_file)
  : AbstractPointCloudImporter(input_file)
{
}

bool AssimpImporter::import_implementation()
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFile(input_file,
                                           aiProcess_PreTransformVertices |
                                           aiProcess_ImproveCacheLocality);

  if(scene == nullptr)
  {
    print_error(importer.GetErrorString());
    return false;
  }

  size_t num_vertices = 0;

  for(uint mesh_index=0; mesh_index<scene->mNumMeshes; ++mesh_index)
    num_vertices += scene->mMeshes[mesh_index]->mNumVertices;

  print("num_vertices ", num_vertices);

  pointcloud.resize(num_vertices);

  size_t begin_subrange = 0;
  for(uint mesh_index=0; mesh_index<scene->mNumMeshes; ++mesh_index)
  {
    const aiMesh& mesh = *scene->mMeshes[mesh_index];

    const uint subrange_length = mesh.mNumVertices;

    static_assert(std::is_same<float32_t, decltype(mesh.mVertices->x)>::value, "Need to update the data type below");
    static_assert(std::is_same<float32_t, decltype(mesh.mColors[0]->r)>::value, "Need to update the data type below");

    const data_type_t vertex_input_type{data_type_t::BASE_TYPE::FLOAT32, 3, sizeof(aiVector3D)};
    const data_type_t color_input_type{data_type_t::BASE_TYPE::FLOAT32, 3, sizeof(aiColor4D)};

    pointcloud.set_data(PointCloud::COLUMN::COORDINATES, vertex_input_type, reinterpret_cast<const unsigned  char*>(mesh.mVertices), begin_subrange, subrange_length);

    if(mesh.mColors[0] != nullptr)
      pointcloud.set_data(PointCloud::COLUMN::COLOR, color_input_type, reinterpret_cast<const unsigned  char*>(mesh.mColors[0]), begin_subrange, subrange_length);

    begin_subrange += subrange_length;
  }

  pointcloud.aabb = aabb_t::invalid();

  const PointCloud::vertex_t* vertices = reinterpret_cast<const PointCloud::vertex_t*>(pointcloud.coordinate_color.data());
  for(size_t i=0; i<num_vertices; ++i)
  {
    pointcloud.aabb |= vertices[i].coordinate;
  }

  return true;
}

#endif
