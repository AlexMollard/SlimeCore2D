#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "gtc/matrix_transform.hpp"
#include "Texture.h"
#include "RigidBody.h"

class GameObject : public RigidBody
{
public:
	GameObject() {};
	GameObject(Mesh* mesh, Shader* shader, Texture* tex = nullptr);
	~GameObject();

	void Draw();
	virtual void Update(float deltaTime);

	virtual void Create(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int id);

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

	Texture* GetTexture();
	void SetTexture(Texture* tex);

	int type = 0;

protected:
	int ID = -404;
	bool isHeld = false;
	bool release = false;

	glm::vec3 color = glm::vec3(1);
	glm::vec3 defaultColor = glm::vec3(1);
	
	glm::vec3 scale = glm::vec3(1);

	Shader* shader = nullptr;
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
};

