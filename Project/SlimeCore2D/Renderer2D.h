#pragma once
#include <vector>
#include "GameObject.h"

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
	GameObject* CreateQuad(glm::vec3 pos, glm::vec3 color = glm::vec3(1), glm::vec3 scale = glm::vec3(1));
	void Draw();

private:
	Renderer2D();
	static Renderer2D* instance;

	glm::mat4 orthoMatrix = glm::ortho<float>(-16, 16, -9, 9, -1, 1);

	Shader* currentShader = nullptr;

	std::vector<GameObject*> objectPool;
	std::vector<Shader*> shaderPool;

	Shader* basicShader = nullptr;
	Mesh* cube = nullptr;
};

