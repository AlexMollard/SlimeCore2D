#pragma once
#include "RigidBody.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class GameObject : public RigidBody
{
public:
	GameObject(Mesh* mesh, Shader* shader, Texture* tex = nullptr);
	~GameObject();

	void Draw();
	virtual void Update(float deltaTime);

	virtual void Create(glm::vec3 pos, glm::vec3 color, glm::vec3 scale, int id);

	Shader* GetShader();
	void SetShader(Shader* newShader);
	
	Mesh* GetMesh();
	void SetMesh(Mesh* newMesh);

	void Respawn();
	void SetSpawn(glm::vec2 newSpawn);

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
	glm::vec2 spawnPoint = glm::vec2(0);
	glm::vec3 scale = glm::vec3(1);

	Shader* shader = nullptr;
	Mesh* mesh = nullptr;
	Texture* texture = nullptr;
};

