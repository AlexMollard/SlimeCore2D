#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "BoundingBox.h"
#include "gtc/matrix_transform.hpp"
#include "Texture.h"

class GameObject
{
public:
	GameObject() {};
	GameObject(Mesh* mesh, Shader* shader, Texture* tex = nullptr);
	~GameObject();

	void Draw();
	virtual void Update(float deltaTime);

	virtual void Create(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int id);

	void SetPos(glm::vec2 newPos);
	void SetPos(float x, float y);

	Shader* GetShader();
	void SetShader(Shader* newShader);
	
	Mesh* GetMesh();
	void SetMesh(Mesh* newMesh);

	glm::vec3 GetColor();
	void SetColor(glm::vec3 newColor);
	void SetColor(float r, float g, float b);

	glm::vec3 GetScale();
	void SetScale(glm::vec3 newScale);

	void SetID(int id);
	int GetID();

	BoundingBox* GetBoundingBox();

	Texture* GetTexture();
	void SetTexture(Texture* tex);

protected:
	int ID = -404;
	bool isHeld = false;

	InputManager* inputManager = InputManager::GetInstance();

	glm::vec3 color = glm::vec3(1);
	glm::vec3 defaultColor = glm::vec3(1);
	
	glm::mat4 model = glm::mat4(1);
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	BoundingBox boundingBox;

	Shader* shader = nullptr;
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
};

