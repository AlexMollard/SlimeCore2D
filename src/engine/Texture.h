#pragma once
#include <string>
#include "glew.h"
#include "glfw3.h"

class Texture
{
public:
	Texture(std::string dir);
	Texture(unsigned int id);
	Texture() = default;
	~Texture();

	void load(std::string dir);
	void Bind();;
	unsigned int GetID();;
	void SetID(unsigned int newID);;

	int GetWidth();
	int GetHeight();

protected:
	unsigned int m_textureId = 0;
	int m_width = 0;
	int m_height = 0;
	int m_channels = 4;
};
