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

	static void setActiveRegion(Texture* texture, int regionIndex, int spriteWidth);

	static void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color);
	static void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, Texture* texture, int frame = 0, int spriteWidth = 16);

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

	static Shader* basicShader;
	static Camera* camera;
	MeshManager* meshManager = nullptr;

	static std::vector<glm::vec2> UVs;
};