#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;

uniform sampler2D Textures[31];

uniform vec3 color;

uniform float GradientAmount;

void main()
{
	int index = int(TexIndex);

	vec4 textureColor = texture(Textures[index], TexCoord);
	if (textureColor.a < 0.01 || textureColor.r > GradientAmount || textureColor.r == 1.0)
		discard;

	FragColor = (texture(Textures[index], TexCoord) * Color);
}