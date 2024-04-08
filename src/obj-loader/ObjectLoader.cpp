#define TINYOBJLOADER_IMPLEMENTATION
#include "ObjectLoader.hpp"

#include <iostream>

namespace ObjectLoader {

MyModel loadObjModel(std::string const &&path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  std::cout << "Loading model: " << path << std::endl;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
    std::cout << warn << err << std::endl;
    throw std::runtime_error(warn + err);
  }

  std::cout << "Model loaded successfully, with " << shapes.size() << " shapes" << std::endl;

  MyModel model;
  for (const auto &shape : shapes) {
    std::cout << "Shape " << shape.name << " has " << shape.mesh.indices.size() << " indices"
              << std::endl;
    for (const auto &index : shape.mesh.indices) {
      MyVertex vertex{};

      vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};

      // vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
      //                    attrib.texcoords[2 * index.texcoord_index + 1]};

      vertex.color = {1.0f, 1.0f, 1.0f};

      model.vertices.push_back(vertex);
      model.indices.push_back(model.indices.size());
    }
  }

  std::cout << "Model has " << model.vertices.size() << " vertices" << std::endl;

  return model;
}

} // namespace ObjectLoader
