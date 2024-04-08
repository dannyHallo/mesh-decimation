// Includes the daxa shader API
#include <daxa/daxa.inl>

// Enabled the extension GL_EXT_debug_printf
#extension GL_EXT_debug_printf : enable

#include "PushConstants.inl"

// Enabled the push constant MyPushConstant we specified in shared.inl
DAXA_DECL_PUSH_CONSTANT(MyPushConstant, push)

// We can define the vertex & fragment shader in one single file
#if DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_VERTEX

layout(location = 0) out daxa_f32vec3 v_col;
void main()
{
    G_Vertex vert = deref(push.vertexPtr[gl_VertexIndex]);
    
    G_Ubo ubo = deref(push.uboPtr);
    
    gl_Position = ubo.vpMat * ubo.modelMat * daxa_f32vec4(vert.position, 1);
    v_col = vert.color;
}

#elif DAXA_SHADER_STAGE == DAXA_SHADER_STAGE_FRAGMENT

layout(location = 0) in daxa_f32vec3 v_col; 
layout(location = 0) out daxa_f32vec4 color;
void main()
{
    color = daxa_f32vec4(v_col, 1);
    // color = daxa_f32vec4(1, 0, 0, 1);
    debugPrintfEXT("test\n");
}

#endif