#pragma once

#include "shared/PushConstants.inl"
#include "tiny_obj_loader.h"

#include <string>

struct MyModel {
  std::vector<G_Vertex> vertices;
  std::vector<size_t> indices;
};

namespace ObjectLoader {
MyModel loadObjModel(std::string const &&path);

}; // namespace ObjectLoader
