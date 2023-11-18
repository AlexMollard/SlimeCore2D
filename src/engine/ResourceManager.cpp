
#include "ResourceManager.h"
const std::string ResourceManager::BASE_TEXTURE_PATH = "../resources/textures/";
const std::string ResourceManager::BASE_SHADER_PATH = "../resources/shaders/";

std::string ResourceManager::GetTexturePath(const std::string& name, const std::string& extension)
{
	return BASE_TEXTURE_PATH + name + extension;
}

std::string ResourceManager::GetShaderPath(const std::string& name, const std::string& extension)
{
	return BASE_SHADER_PATH + name + extension;
}
