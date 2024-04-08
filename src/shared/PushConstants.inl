#pragma once

#include "daxa/daxa.inl"

struct G_Vertex {
  daxa_f32vec3 position;
  daxa_f32vec2 texCoord;
  daxa_f32vec3 color;
};
DAXA_DECL_BUFFER_PTR(G_Vertex)

struct G_Ubo {
  daxa_f32mat4x4 vpMat;
  daxa_f32mat4x4 modelMat;
};
DAXA_DECL_BUFFER_PTR(G_Ubo)

struct MyPushConstant {
  daxa_BufferPtr(G_Vertex) vertexPtr;
  daxa_BufferPtr(G_Ubo) uboPtr;
};
