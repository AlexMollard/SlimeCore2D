#version 450

// Pass the colors of the vertices to the fragment shader
layout (location = 0) out vec4 inColor;

void main() {
	//const array of positions for the triangle
    const vec3 positions[3] = vec3[3](vec3(0.5, 0.5, 0.0), vec3(-0.5, 0.5, 0.0), vec3(0.0, -0.5, 0.0));
    const vec4 colors[3] = vec4[3](vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0));
    
    //output the color of each vertex
    inColor = colors[gl_VertexIndex];

	//output the position of each vertex
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}
