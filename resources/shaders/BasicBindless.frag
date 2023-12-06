#version 430

#extension GL_GOOGLE_include_directive : require
#include "bindless.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inViewDir;
layout (location = 4) in vec3 inWorldPos;

layout (location = 0) out vec4 outFragColor;

void main() {
    outFragColor = vec4(inColor, 1.0);
}