#version 450

#extension GL_GOOGLE_include_directive : require

layout(set = 0, binding = 0) uniform  SceneData{   

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambientColor;
	vec4 sunlightDirection; //w for sun power
	vec4 sunlightColor;
} sceneData;

layout(set = 0, binding = 1) uniform sampler2D depthTex;   

layout(set = 1, binding = 0) uniform GLTFMaterialData{   

	vec4 colorFactors;
	vec4 metal_rough_factors;
	
} materialData;

// We need the albedo and depth textures from the GBuffer
layout(set = 1, binding = 1) uniform sampler2D albedoTex;


layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inViewDir;
layout (location = 4) in vec3 inWorldPos;

layout (location = 0) out vec4 outFragColor;

void main() {
    // Sample textures
    vec4 albedo = texture(albedoTex, inUV);

    // Compute basic lighting
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(inViewDir);
    vec3 lightDir = normalize(vec3(sceneData.sunlightDirection));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = sceneData.sunlightColor.rgb * diff * sceneData.sunlightDirection.w;

    // Apply cell shading
    float numLevels = 4.0; // Adjust the number of shading levels as needed
    float threshold = 1.0 - 1.0 / numLevels;
    float intensity = smoothstep(threshold - 0.01, threshold + 0.01, diff);

    // Final color
    vec3 shadedColor = albedo.rgb * (diffuse + sceneData.ambientColor.rgb);
    outFragColor = vec4(shadedColor * intensity, albedo.a);
}