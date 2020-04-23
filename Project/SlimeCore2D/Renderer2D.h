#pragma once
#include "Quad.h"
#include "Texture.h"
#include <vector>
#include "MeshManager.h"
#include "Camera.h"

class Renderer2D
{
public:
	Renderer2D(MeshManager* meshManager, Camera* camera);
	~Renderer2D();

	static void Init();
	static void ShutDown();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(std::string dir);

	void Draw();
 
	Shader* GetBasicShader();

	static void setActiveRegion(Texture* texture, int regionIndex);

	static void DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color);
	static void DrawQuad(glm::vec2 position, glm::vec2 size, Texture* texture, int frame = 0);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();


private:
	glm::vec3 currentColor = glm::vec3(-404);

	Shader* currentShader = nullptr;
	Texture* currentTexture = nullptr;

	std::vector<GameObject*> objectPool;
	std::vector<Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	Shader* basicShader = nullptr;
	Camera* camera = nullptr;
	MeshManager* meshManager = nullptr;

	static std::vector<glm::vec2> UVs;
};