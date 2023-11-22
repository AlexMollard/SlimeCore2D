#include "ResourceManager.h"

#include "ConsoleLog.h"
#include "Constants.h"
#include "Shader.h"
#include "Texture.h"

#include "engine/MemoryDebugging.h"

const std::string ResourceManager::BASE_TEXTURE_PATH = "../resources/textures/";
const std::string ResourceManager::BASE_SHADER_PATH  = "../resources/shaders/";
const std::string ResourceManager::BASE_FONT_PATH    = "../resources/fonts/";

std::string ResourceManager::GetTexturePath(const std::string& name, const std::string& extension)
{
	return BASE_TEXTURE_PATH + name + extension;
}

std::string ResourceManager::GetShaderPath(const std::string& name, const std::string& extension)
{
	return BASE_SHADER_PATH + name + extension;
}

std::string ResourceManager::GetFontPath(const std::string& name, const std::string& extension) 
{
	return BASE_FONT_PATH + name + extension;
}

ResourceManager::~ResourceManager()
{
	ReleaseAllResources();
}

Texture* ResourceManager::LoadTexture(const char* texPath)
{
	// Check if the resource is already loaded
	if (m_resourceMap.find(texPath) != m_resourceMap.end())
	{
		// SLIME_INFO("Texture '{0}' is already loaded.", texPath);

		// Return the cached resource
		return static_cast<Texture*>(m_resourceMap[texPath].resource);
	}

	// Dynamically allocate the Texture object
	Texture* texture = new Texture(texPath);

	// Cache the resource
	Resource resource{ texture, texPath, ResourceType::Texture };
	m_resourceMap[texPath] = resource;

	SLIME_INFO("Texture '{0}' loaded and cached.", texPath);

	return texture;
}

Texture* ResourceManager::LoadTexture(const std::string& texPath)
{
	return LoadTexture(texPath.c_str());
}

Shader* ResourceManager::LoadShader(const char* name, const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	// Check if the resource is already loaded
	if (m_resourceMap.find(name) != m_resourceMap.end())
	{
		// SLIME_INFO("Shader '{0}' is already loaded.", name);

		// Return the cached resource
		return static_cast<Shader*>(m_resourceMap[name].resource);
	}

	Shader* shader = new Shader(vertexPath, fragmentPath, geometryPath);

	// Cache the resource
	Resource resource{ shader, name, ResourceType::Shader };
	m_resourceMap[name] = resource;

	SLIME_INFO("Shader '{0}' loaded and cached.", name);

	return shader;
}

TTF_Font* ResourceManager::LoadFont(const char* name, const char* fontPath, int size)
{
	return LoadFont(std::string(name), std::string(fontPath), size);
}

TTF_Font* ResourceManager::LoadFont(const std::string& name, const std::string& fontPath, int size)
{
	// Check if the resource is already loaded
	if (m_resourceMap.find(name) != m_resourceMap.end())
	{
		// SLIME_INFO("Font '{0}' is already loaded.", name);

		// Return the cached resource
		return static_cast<TTF_Font*>(m_resourceMap[name].resource);
	}

	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
	if (font == nullptr)
	{
		SLIME_ERROR("Failed to load font: {}", TTF_GetError());
		return nullptr;
	}

	 TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	// Cache the resource
	Resource resource{ font, name, ResourceType::Font };
	m_resourceMap[name] = resource;

	SLIME_INFO("Font '{0}' loaded and cached.", name);

	return font;
}

void ResourceManager::AddResource(Texture* resource, const std::string& name)
{
	Resource res{ resource, name, ResourceType::Texture };
	m_resourceMap[name] = res;

	SLIME_INFO("Texture '{0}' added to cache.", name);
}

void ResourceManager::AddResource(Shader* resource, const std::string& name)
{
	Resource res{ resource, name, ResourceType::Shader };
	m_resourceMap[name] = res;

	SLIME_INFO("Shader '{0}' added to cache.", name);
}

Resource* ResourceManager::GetResource(const std::string& name)
{
	// Check if the resource is in the map
	auto it = m_resourceMap.find(name);
	if (it != m_resourceMap.end())
	{
		return &(it->second); // Return a pointer to the cached resource
	}

	// Resource not found
	SLIME_ERROR("Resource '{0}' not found.", name);
	return nullptr;
}

Resource* ResourceManager::GetResource(const char* name)
{
	return GetResource(std::string(name));
}

void ResourceManager::ReleaseResource(const std::string& name)
{
	auto it = m_resourceMap.find(name);
	if (it != m_resourceMap.end())
	{
		if (it->second.type == ResourceType::Texture)
			delete static_cast<Texture*>(it->second.resource);
		else if (it->second.type == ResourceType::Shader)
			delete static_cast<Shader*>(it->second.resource);
		else if (it->second.type == ResourceType::Font)
			TTF_CloseFont(static_cast<TTF_Font*>(it->second.resource));

		m_resourceMap.erase(it);
		SLIME_INFO("Resource '{0}' released.", name);
	}
	else
	{
		SLIME_INFO("Resource '{0}' not found.", name);
	}
}

void ResourceManager::ReleaseAllResources()
{
	for (auto& resource : m_resourceMap)
	{
		if (resource.second.type == ResourceType::Texture)
			delete static_cast<Texture*>(resource.second.resource);
		else if (resource.second.type == ResourceType::Shader)
			delete static_cast<Shader*>(resource.second.resource);
		else if (resource.second.type == ResourceType::Font)
			TTF_CloseFont(static_cast<TTF_Font*>(resource.second.resource));
	}

	m_resourceMap.clear();
	SLIME_INFO("All resources released.");
}
