#version 430

#extension GL_GOOGLE_include_directive : require
#include "bindless.glsl"

RegisterUniform(Camera, {
  mat4 viewProjection;
  mat4 view;
  mat4 projection;
});

struct TransformData {
  mat4 transform;
};

RegisterBuffer(std430, readonly, Transforms, {
  TransformData items[];
});


layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewDir;
layout (location = 4) out vec3 outWorldPos;

void main() {
  mat4 viewProjection = GetResource(Camera, 5).viewProjection;
  TransformData transform = GetResource(Transforms, 0).items[gl_InstanceIndex];
  gl_Position = viewProjection * transform.transform * vec4(0, 0, 0, 1);
}