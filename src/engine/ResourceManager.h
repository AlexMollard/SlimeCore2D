#pragma once
#include <string>
#include "Constants.h"

class ResourceManager {
public:
	static const std::string BASE_TEXTURE_PATH;
	static const std::string BASE_SHADER_PATH;

	static std::string GetTexturePath(const std::string& name, const std::string& extension = ".png");
	
	static std::string GetShaderPath(const std::string& name, const std::string& extension = ".shader");
};


