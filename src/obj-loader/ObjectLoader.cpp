#define TINYOBJLOADER_IMPLEMENTATION
#include "ObjectLoader.hpp"

namespace ObjectLoader {

MyModel loadObjModel(std::string const &&path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
    throw std::runtime_error(warn + err);
  }

  MyModel model;
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      MyVertex vertex{};

      vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                         attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      model.vertices.push_back(vertex);
      model.indices.push_back(model.indices.size());
    }
  }

  return model;
}

} // namespace ObjectLoader
