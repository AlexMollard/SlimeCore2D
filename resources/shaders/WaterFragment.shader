#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;

uniform sampler2D Textures[5];

int sceneTexture = 1;
int waterNormalMap = 2;
int waterDepthMap = 3;
int causticsTexture = 4;

uniform float time; // Current time.
uniform vec2 lightDirection; // Direction of the light source.

const float waveSpeed = 0.02; // Speed of the waves.
const float waveAmplitude = 5.0; // Amplitude of the waves.
const float reflectionStrength = 0.3; // Strength of reflections.
const float refractionStrength = 0.1; // Strength of refractions.
const float causticsSpeed = 0.01; // Speed of caustics animation.

void main() {
    // Calculate the offset for scrolling the water texture.
    vec2 offset = vec2(0.0, time * waveSpeed);

    // Calculate the distorted texture coordinates for the water.
    vec2 distortedCoords = TexCoord + offset;

    // Apply wave distortion using the water normal map.
    vec3 normal = texture(Textures[waterNormalMap], distortedCoords).xyz;
    vec2 normalOffset = (normal.xy * 2.0 - 1.0) * waveAmplitude;
    distortedCoords += normalOffset;

    // Sample the water texture using the distorted coordinates.
    vec4 waterColor = texture(Textures[sceneTexture], distortedCoords);

    // Calculate reflection texture coordinates.
    vec2 reflectionCoords = vec2(TexCoord.x, 1.0 - TexCoord.y);

    // Sample the reflected scene texture.
    vec4 reflectionColor = texture(Textures[sceneTexture], reflectionCoords);

    // Calculate refraction texture coordinates.
    vec2 refractionCoords = TexCoord - 0.02 * normalize(lightDirection);

    // Sample the refracted scene texture.
    vec4 refractionColor = texture(Textures[sceneTexture], refractionCoords);

    // Apply depth-based refraction.
    float depth = texture(Textures[waterDepthMap], TexCoord).r;
    vec2 depthOffset = (normal.xy * 2.0 - 1.0) * depth;
    refractionCoords += depthOffset;

    // Sample the refracted scene texture with depth-based offset.
    refractionColor = texture(Textures[sceneTexture], refractionCoords);

    // Calculate caustics animation.
    vec2 causticsOffset = vec2(causticsSpeed * time, 0.0);
    vec4 caustics = texture(Textures[causticsTexture], TexCoord + causticsOffset);

    // Combine the water color, reflection, refraction, and caustics.
    vec4 finalColor = mix(waterColor, reflectionColor, reflectionStrength);
    finalColor = mix(finalColor, refractionColor, refractionStrength);
    finalColor.rgb += caustics.rgb;

    // Set the final color to the combined color.
    FragColor = finalColor;
}
