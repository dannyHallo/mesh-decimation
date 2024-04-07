#pragma once

#include "daxa/daxa.inl"

struct MyVertex {
  daxa_f32vec3 position;
  daxa_f32vec2 texCoord;
  daxa_f32vec3 color;
};
DAXA_DECL_BUFFER_PTR(MyVertex)

struct MyCameraTransform {
  daxa_f32mat4x4 vpMat;
};
DAXA_DECL_BUFFER_PTR(MyCameraTransform)

struct MyPushConstant {
  daxa_BufferPtr(MyVertex) my_vertex_ptr;
  daxa_BufferPtr(MyCameraTransform) my_camera_transform_ptr;
};
