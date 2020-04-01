#pragma once
#include "Quad.h"
#include "gtc/matrix_transform.hpp"
#include "Mesh.h"
#include "TextRenderer.h"
#include "Texture.h"
#include <vector>

class Renderer2D
{
public:
	Renderer2D();
	~Renderer2D();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(std::string dir);
	Shader* GetShaderFromtype(ObjectType type);

	void Draw();

	Mesh* GetQuadMesh();
	Mesh* GetCircleMesh();
	Shader* GetBasicShader();

private:
	glm::mat4 orthoMatrix = glm::ortho<float>(-32, 32, -18, 18, -1, 1);

	Shader* currentShader = nullptr;
	Texture* currentTexture = nullptr;
	TextRenderer* textRenderer = nullptr;

	std::vector<GameObject*> objectPool;
	std::vector<Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	Shader* basicShader = nullptr;
	Mesh* quad = nullptr;
	Mesh* circle = nullptr;
};