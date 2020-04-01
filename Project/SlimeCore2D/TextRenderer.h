#pragma once
#include "glm.hpp"
#include "glew.h"
#include "glfw3.h"
#include "Shader.h"

#define GLT_IMPLEMENTATION
#include "gltext.h"

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();

	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:

	Shader* shader = nullptr;
};
