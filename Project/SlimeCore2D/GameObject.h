#pragma once
#include "Mesh.h"
#include "Shader.h"

class GameObject
{
public:
	GameObject(Mesh* mesh, Shader* shader);
	~GameObject();

	void Draw();
	void Update(float deltaTime);

	void SetPos(glm::vec2 newPos);
	void SetPos(float x, float y);

	Shader* GetShader();
	void SetShader(Shader* newShader);
	
	Mesh* GetMesh();
	void SetMesh(Mesh* newMesh);

	glm::vec3 GetColor();
	void SetColor(glm::vec3 newColor);
	void SetColor(float r, float g, float b);

protected:
	glm::vec3 color = glm::vec3(1);
	
	glm::mat4 model = glm::mat4(1);
	glm::vec3 position = glm::vec3(0);

	Shader* shader = nullptr;
	Mesh* mesh = nullptr;
};

