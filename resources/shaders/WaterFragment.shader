#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;

uniform sampler2D Textures[5]; // White, render target, simplex noise, simplex noise soomed in, simplex noise soomed in more

uniform float LerpProgress;

void main()
{
	if (texture(Textures[1], TexCoord).a < 0.01)
		discard;

	// Apply a water effect
	vec4 simplexNoise = texture(Textures[2], TexCoord);
	vec4 simplexNoiseZoomed = texture(Textures[3], TexCoord);
	vec4 simplexNoiseZoomedMore = texture(Textures[4], TexCoord);

	// mix and lerp the colors
	vec4 waterColor = mix(Color, vec4(0.0, 0.0, 1.0, 1.0), simplexNoise.r);
	vec4 waterColorZoomed = mix(Color, vec4(0.0, 0.0, 1.0, 1.0), simplexNoiseZoomed.r);
	vec4 waterColorZoomedMore = mix(Color, vec4(0.0, 0.0, 1.0, 1.0), simplexNoiseZoomedMore.r);

	// Apply the lerp progress
	waterColor = mix(Color, waterColor, LerpProgress);
	waterColorZoomed = mix(Color, waterColorZoomed, LerpProgress);
	waterColorZoomedMore = mix(Color, waterColorZoomedMore, LerpProgress);

	// Apply the water effect
	FragColor = mix(waterColor, waterColorZoomed, simplexNoiseZoomed.r);
	FragColor = mix(FragColor, waterColorZoomedMore, simplexNoiseZoomedMore.r);

	// Apply the lerp progress
	FragColor = mix(Color, FragColor, LerpProgress);
}