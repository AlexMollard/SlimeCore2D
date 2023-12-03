#version 450

#extension GL_EXT_scalar_block_layout : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require
//#include "scene_data.glsl"

#include "InputStructures.glsl"

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewDir;
layout (location = 4) out vec3 outWorldPos;

struct Vertex {
    vec3 position;
    float uv_x;
    vec3 normal;
    float uv_y;
    vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer { 
    Vertex vertices[];
};

// push constants block
layout(push_constant) uniform constants {
    mat4 render_matrix;
    VertexBuffer vertexBuffer;
} PushConstants;

void main() {
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

    vec4 position = vec4(v.position, 1.0f);

    gl_Position = sceneData.viewproj * PushConstants.render_matrix * position;

    outNormal = normalize((PushConstants.render_matrix * vec4(v.normal, 0.f)).xyz);
    outColor = v.color.xyz * materialData.colorFactors.xyz;
    
    // Correct UV coordinates
    outUV.x = v.uv_x;
    outUV.y = 1.0 - v.uv_y; // Invert Y coordinate if needed (depends on the UV conventions)

    outViewDir = normalize((sceneData.view * position).xyz);
    
    // Calculate world-space position
    outWorldPos = (PushConstants.render_matrix * position).xyz;
}
