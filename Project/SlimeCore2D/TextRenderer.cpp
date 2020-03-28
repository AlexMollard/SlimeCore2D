#include "TextRenderer.h"
#include <iostream>

TextRenderer::TextRenderer()
{
	// Initialize glText
	gltInit();
}

TextRenderer::~TextRenderer()
{
	// Destroy glText
	gltTerminate();
}

void TextRenderer::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Creating text
	GLTtext* writeText = gltCreateText();
	gltSetText(writeText, text.c_str());

	// Begin text drawing (this for instance calls glUseProgram)
	gltBeginDraw();

	// Draw any amount of text between begin and end
	gltColor(color.x, color.y, color.z, 1.0f);
	gltDrawText2D(writeText, x, y, scale);

	// Finish drawing text
	gltEndDraw();

	// Deleting text
	gltDeleteText(writeText);
}