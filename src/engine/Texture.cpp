

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "glm.hpp"
#include "stb_image.h"
//#include "stb_image_write.h"

#define FNL_IMPL
#include "Noise.h"

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

Texture::Texture(float* data, int width, int height)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);

	m_width  = width;
	m_height = height;

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

void Texture::GenerateNoise(NoiseType noiseType, int width, int height, float scale, float offsetX, float offsetY)
{
	fnl_state noise = fnlCreateState();

	switch (noiseType)
	{
	case NoiseType::Perlin: noise.noise_type = FNL_NOISE_PERLIN; break;
	case NoiseType::Simplex: noise.noise_type = FNL_NOISE_OPENSIMPLEX2; break;
	case NoiseType::Cellular: noise.noise_type = FNL_NOISE_CELLULAR; break;
	default: noise.noise_type = FNL_NOISE_PERLIN; break;
	}

	float* noiseMap = new float[width * height * 3]; // RGB each channel being a diffrent scale of noise

	noise.seed                   = rand() % 1000000;
	noise.frequency              = scale;
	noise.octaves                = 1;
	noise.lacunarity             = 2.0f;
	noise.gain                   = 0.5f;
	noise.cellular_distance_func = FNL_CELLULAR_DISTANCE_EUCLIDEAN;
	noise.cellular_return_type   = FNL_CELLULAR_RETURN_VALUE_DISTANCE2;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			noise.frequency                   = scale * 0.33f;
			noiseMap[(y * width + x) * 3 + 0] = fnlGetNoise2D(&noise, x + offsetX, y + offsetY);

			noise.frequency                   = scale * 0.66f;
			noiseMap[(y * width + x) * 3 + 1] = fnlGetNoise2D(&noise, x + offsetX, y + offsetY);

			noise.frequency                   = scale;
			noiseMap[(y * width + x) * 3 + 2] = fnlGetNoise2D(&noise, x + offsetX, y + offsetY);
		}
	}

	// Create and bind a new texture
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// Set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload the noise data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, noiseMap);

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Save the noise map to a PNG for debugging
	//stbi_write_png("noise.png", width, height, 3, noiseMap, 0);

	// Don't forget to delete the noiseMap when you're done with it!
	delete[] noiseMap;
}

void Texture::GenerateColor(glm::vec4 colour, int width, int height)
{
	float* colorMap = new float[width * height * 4]; // RGBA

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			colorMap[(y * width + x) * 4 + 0] = colour.r;
			colorMap[(y * width + x) * 4 + 1] = colour.g;
			colorMap[(y * width + x) * 4 + 2] = colour.b;
			colorMap[(y * width + x) * 4 + 3] = colour.a;
		}
	}

	// Create and bind a new texture
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// Set the texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload the pixel colour data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, colorMap);

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Save the color map to a PNG for debugging
	//stbi_write_png("color.png", width, height, 4, colorMap, 0);

	// Don't forget to delete the colorMap when you're done with it!
	delete[] colorMap;
}

// Function to calculate the distance between two points
float distance(float x1, float y1, float x2, float y2)
{
	return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void Texture::GenerateRoundedMask(float borderRadius, int width, int height)
{
	// Allocate memory for the mask map
	unsigned char* maskMap = new unsigned char[4 * width * height];

    // Calculate the center of the image
	float centerX = width / 2.0f;
	float centerY = height / 2.0f;

	// Iterate through each pixel
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// Calculate the distance from the pixel to the center
			float dist = distance(x, y, centerX, centerY);

			// Calculate alpha based on the distance and corner radius
			float alpha = std::max(0.0f, 1.0f - (dist - borderRadius) / borderRadius);

			// Clamp alpha values to [0, 1]
			alpha = std::min(1.0f, alpha);

			// Set alpha in the mask map
			unsigned char alphaByte = static_cast<unsigned char>((alpha < 0.75f) ? 0 : 255);
			
			int index               = 4 * (y * width + x);

			// Store alpha in the mask map
			maskMap[index]     = alphaByte; // Red channel
			maskMap[index + 1] = alphaByte; // Green channel
			maskMap[index + 2] = alphaByte; // Blue channel
			maskMap[index + 3] = alphaByte; // Alpha channel
		}
	}

	// Generate and bind a new texture
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Upload mask map to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, maskMap);

	// Unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// Save the mask map to a PNG for debugging
	//stbi_write_png("mask.png", width, height, 4, maskMap, 0);

	// Don't forget to delete the mask map when you're done with it!
	delete[] maskMap;
}