#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 0) uniform sampler2D textureSampler;

layout(location = 0) out vec4 FragColor;

void main()
{
    // Sample texture
    vec4 texColor = texture(textureSampler, fragTexCoord);

    // Output final color by multiplying texture color and vertex color
    FragColor = vec4(texColor.rgb * fragColor, texColor.a);
}
