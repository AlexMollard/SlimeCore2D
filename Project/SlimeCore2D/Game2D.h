#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"
#include "gtc/matrix_transform.hpp"

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

private:
	glm::mat4 orthoMatrix = glm::ortho<float>(-16, 16, -9, 9, -1, 1);

	Shader* testShader = nullptr;
	Mesh* cube = nullptr;

	GameObject* go = nullptr;

	float timer = 0.0f;
};

