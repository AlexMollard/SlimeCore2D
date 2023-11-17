#version 450

layout(location = 0) out vec4 FragColor;

in vec4 Color;
in vec2 TexCoord;
in float TexIndex;
in float MaskIndex;

uniform sampler2D Textures[31];

void main()
{
    // Sample textures
    int index = int(TexIndex);
    vec4 albedo = texture(Textures[index], TexCoord);

    // If fully transparent, discard the fragment
    if (albedo.a <= 0.0f)
        discard;

    // Check if MaskIndex is non-zero
    if (MaskIndex >= 1.0)
    {
        int maskIndex = int(MaskIndex);
        float mask = texture(Textures[maskIndex], TexCoord).r;

		albedo *= mask.r;
    }

    FragColor = albedo * Color;
}