#version 450

out vec4 FragColor;
in vec2 TexCoords;

layout(location = 0)uniform sampler2D Texture;
uniform vec3 color;

void main()
{
	if (texture(Texture, TexCoords).a < 0.1)
		discard;

    vec3 col = vec3(texture(Texture, TexCoords).rgb) * color;
    FragColor = vec4(color, 1.0);
}