#pragma once
#include <vector>
#include "Button.h"
#include "TextRenderer.h"
class Renderer2D
{
public:
	static Renderer2D* GetInstance() {
		if (!instance)
			instance = new Renderer2D;
		return instance;
	};

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
	Renderer2D();
	static Renderer2D* instance;

	glm::mat4 orthoMatrix = glm::ortho<float>(-16, 16, -9, 9, -1, 1);

	Shader* currentShader = nullptr;
	Texture* currentTexture = nullptr;
	TextRenderer* textRenderer = nullptr;

	std::vector<GameObject*> objectPool;
	std::vector<Texture*> texturePool;
	std::vector<Shader*> shaderPool;

	Shader* basicShader = nullptr;
	Mesh* quad = nullptr;
};