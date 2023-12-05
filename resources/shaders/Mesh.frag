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
    // Get the albedo and depth from the GBuffer
    vec4 albedo = texture(albedoTex, inUV);
    float depth = texture(depthTex, inUV).r;

    // Calculate the world position from the depth
    vec4 worldPos = vec4(inUV * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    mat4 projInverse = inverse(sceneData.proj);
    worldPos = projInverse * worldPos;
    worldPos /= worldPos.w;

    // Calculate the view direction
    vec3 viewDir = normalize(inViewDir);

    // Calculate the normal
    vec3 normal = normalize(inNormal);

    // Calculate the light direction
    vec3 lightDir = normalize(sceneData.sunlightDirection.xyz);

    // Calculate the diffuse color
    vec3 diffuseColor = albedo.rgb * materialData.colorFactors.rgb;

    // Calculate the specular color
    vec3 specularColor = vec3(0.0);
    if (materialData.metal_rough_factors.x > 0.0) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        float NdotV = max(dot(normal, viewDir), 0.0);
        float NdotL = max(dot(normal, lightDir), 0.0);
        float VdotH = max(dot(viewDir, halfwayDir), 0.0);
        specularColor = materialData.colorFactors.rgb * materialData.metal_rough_factors.x * pow(NdotH, materialData.metal_rough_factors.y);
        float F = 0.0;
        float Fc = pow(1.0 - VdotH, 5.0);
        F = 0.16;
        F *= (1.0 - materialData.metal_rough_factors.x);
        F += Fc * materialData.metal_rough_factors.x;
        specularColor += (vec3(F) - specularColor) * materialData.metal_rough_factors.z;
        specularColor *= NdotL;
    }

    // Calculate the ambient color
    vec3 ambientColor = (sceneData.ambientColor.rgb * diffuseColor) * sceneData.ambientColor.w;

    // Calculate the diffuse color
    vec3 diffuse = diffuseColor * max(dot(lightDir, normal), 0.0);

    // Calculate the specular color
    vec3 specular = specularColor * max(dot(lightDir, normal), 0.0);

    // Calculate the final color
    vec3 finalColor = ambientColor + diffuse + specular;

    // Calculate the final color with sunlight
    vec3 finalSunlightColor = finalColor * sceneData.sunlightColor.rgb * sceneData.sunlightColor.w;

    // Tone map the final color
    finalSunlightColor = finalSunlightColor / (finalSunlightColor + vec3(1.0));

    // Output the final color
    //outFragColor = vec4(finalSunlightColor, 1.0);
outFragColor = vec4(albedo.rgb, 1.0);
}