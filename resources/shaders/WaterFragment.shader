#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;

uniform sampler2D Textures[6];

uniform float LerpProgress;

float lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

// This is the water shader that is used to render the water
void main()
{
	int index = int(TexIndex);

	if (texture(Textures[index], TexCoord).a < 0.01)
		discard;

	// This noise texture contains 3 channels of noise each progressively more zoomed in
	vec4 noiseTexture = texture(Textures[index + 1], TexCoord);

	// This is the noise texture that is used to distort the water
	vec2 noise = vec2(noiseTexture.r, noiseTexture.g);

	// We want to distort the texture coordinates based on the noise
	vec2 distortedTexCoord = TexCoord + noise * 0.01;

	// We want to lerp between the distorted texture coordinates and the original texture coordinates
	vec2 finalTexCoord = mix(TexCoord, distortedTexCoord, LerpProgress);

	// We want to sample the texture at the final texture coordinates
	vec4 finalColor = texture(Textures[index], finalTexCoord);

	// We want to lerp between the original color and the final color
	FragColor = mix(Color, finalColor, LerpProgress) * Color;

}