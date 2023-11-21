#pragma once
#include <string>
#include <unordered_map>

class Texture;
class Shader;

enum class ResourceType
{
	Texture,
	Shader,
	Font,
	Audio,
	Animation,
	Particle,
	Script,
	Level,
	Other
};

struct Resource
{
	void* resource;
	std::string name;
	ResourceType type;
};

// Singleton class
class ResourceManager
{
public:
	static ResourceManager* GetInstance()
	{
		if (!m_instance)
			m_instance = new ResourceManager;
		return m_instance;
	}

	~ResourceManager();

	static const std::string BASE_TEXTURE_PATH;
	static const std::string BASE_SHADER_PATH;

	static std::string GetTexturePath(const std::string& name, const std::string& extension = ".png");
	static std::string GetShaderPath(const std::string& name, const std::string& extension = ".shader");

	// Methods for loading and retrieving resources
	Texture* LoadTexture(const std::string& texPath);
	Texture* LoadTexture(const char* texPath);

	Shader* LoadShader(const char* name, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	// Methods for adding existing resources to the cache
	void AddResource(Texture* resource, const std::string& name);
	void AddResource(Shader* resource, const std::string& name);

	// Method to retrieve a resource by name
	Resource* GetResource(const char* name);
	Resource* GetResource(const std::string& name);
	void ReleaseResource(const std::string& name);
	void ReleaseAllResources();

private:
	static ResourceManager* m_instance;
	ResourceManager() = default;

	// Map to store cached resources
	std::unordered_map<std::string, Resource> m_resourceMap;

	ResourceManager(const ResourceManager&)            = delete;
	ResourceManager(ResourceManager&&)                 = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
};
