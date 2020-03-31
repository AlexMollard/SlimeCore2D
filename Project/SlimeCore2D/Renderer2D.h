#pragma once
#include <vector>
#include "TextRenderer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Button.h"
#include "gtc/matrix_transform.hpp"

class Renderer2D
{
public:
	Renderer2D();
	~Renderer2D();

	void AddObject(GameObject* newObject);
	GameObject* CreateQuad(glm::vec3 pos, glm::vec3 color = glm::vec3(1), glm::vec3 scale = glm::vec3(1), int TexIndex = 0);
	Button* CreateButton(glm::vec3 pos, glm::vec3 color = glm::vec3(1), std::string text = "Button", glm::vec3 scale = glm::vec3(1), int TexIndex = 0);
	Texture* LoadTexture(std::string dir);

	void Draw();
	void Update(float deltaTime);

	Mesh* GetQuadMesh();
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
};