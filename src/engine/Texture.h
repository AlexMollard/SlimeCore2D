#pragma once
#include "glew.h"
#include "glfw3.h"
#include <string>

enum class NoiseType
{
	Perlin,
	Simplex,
	Cellular
};

class Texture
{
public:
	Texture(std::string dir);
	Texture(unsigned int id);
	Texture() = default;
	~Texture();

	void load(std::string dir);
	void Bind();
	unsigned int GetID();
	void SetID(unsigned int newID);

	int GetWidth();
	int GetHeight();
	int GetChannels();

	void SetWidth(int newWidth);
	void SetHeight(int newHeight);
	void SetChannels(int newChannels);

	void GenerateNoise(NoiseType noiseType, int width, int height, float scale, float offsetX, float offsetY);

protected:
	unsigned int m_textureId = 0;
	int m_width              = 0;
	int m_height             = 0;
	int m_channels           = 4;
};
