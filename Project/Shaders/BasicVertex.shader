#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 OrthoMatrix;
uniform mat4 Model;
uniform vec3 position;

void main()
{
    TexCoords = aTexCoords;
    TexCoords.y = -1 * TexCoords.y;
    gl_Position = OrthoMatrix * Model * vec4(aPos.xy, 0.0, 1.0);
}