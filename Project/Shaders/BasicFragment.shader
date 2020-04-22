#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;

uniform sampler2D Textures[21];

uniform vec3 color;

void main()
{
	int index = int(TexIndex);
	FragColor = texture(Textures[index], TexCoord) * Color;
}