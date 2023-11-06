#include "pch.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(std::string dir)
{
	// Create and bind texture ID
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// Set Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load Image and generate mipmaps
	unsigned char* data = stbi_load(dir.c_str(), &m_width, &m_height, &m_channels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, m_channels != 4 ? GL_RGB : GL_RGBA, m_width, m_height, 0, m_channels != 4 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		printf("Failed to load texture: %s\n", dir.c_str());
	}
	stbi_image_free(data);
}

Texture::Texture(unsigned int id) : m_textureId(id)
{
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_height);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	if (m_textureId != 0)
		glDeleteTextures(1, &m_textureId);
	m_textureId = 0;
}

void Texture::load(std::string dir)
{
	// Create and bind texture ID
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// Set Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load Image and generate mipmaps
	unsigned char* data = stbi_load(dir.c_str(), &m_width, &m_height, &m_channels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, m_channels != 4 ? GL_RGB : GL_RGBA, m_width, m_height, 0, m_channels != 4 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		printf("Failed to load texture: %c\n", dir.c_str());
	}
	stbi_image_free(data);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_textureId);
}

unsigned int Texture::GetID()
{
	return m_textureId;
}

void Texture::SetID(unsigned int newID)
{
	m_textureId = newID;
}

int Texture::GetWidth()
{
	return m_width;
}

int Texture::GetHeight()
{
	return m_height;
}

int Texture::GetChannels() 
{
	return m_channels;
}

void Texture::SetWidth(int newWidth) 
{
	m_width = newWidth;
}

void Texture::SetHeight(int newHeight) 
{
	m_height = newHeight;
}

void Texture::SetChannels(int newChannels) 
{
	m_channels = newChannels;
}
