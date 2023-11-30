#version 450

// Get the color from the vertex shader
layout (location = 0) in vec4 inColor;

//output write
layout (location = 0) out vec4 outFragColor;

void main() {
	//return red
	outFragColor = inColor;
}
