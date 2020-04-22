#pragma once
#include "Quad.h"
#include "gtc/matrix_transform.hpp"
#include "Mesh.h"
#include "Texture.h"
#include <vector>
#include "MeshManager.h"

class Renderer2D
{
public:
	Renderer2D(MeshManager* meshManager);
	~Renderer2D();

	static void Init();
	static void ShutDown();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(std::string dir);

	void Draw();
 
	Shader* GetBasicShader();

	static void DrawQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color);
	static void DrawQuad(glm::vec2 position, glm::vec2 size, uint32_t textureID);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

private:
	glm::mat4 orthoMatrix = glm::ortho<float>(-32, 32, -18, 18, -1, 1);
	glm::vec3 currentColor = glm::vec3(-404);

	Shader* currentShader = nullptr;
	Texture* currentTexture = nullptr;

	std::vector<GameObject*> objectPool;
	std::vector<Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	Shader* basicShader = nullptr;

	MeshManager* meshManager = nullptr;
};