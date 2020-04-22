#pragma once
#include "RigidBody.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class GameObject : public RigidBody
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update(float deltaTime);
	virtual void Create(glm::vec2 pos, glm::vec3 color, glm::vec2 scale, int id);

	Mesh* GetMesh();
	void SetMesh(Mesh* newMesh);

	void Respawn();
	void SetSpawn(glm::vec2 newSpawn);

	glm::vec3 GetColor();
	void SetColor(glm::vec3 newColor);
	void SetColor(float r, float g, float b);

	void SetShader(Shader* newShader);
	Shader* GetShader();


	void SetID(int id);
	int GetID();

	Texture* GetTexture();
	void SetTexture(Texture* tex);

	void UpdateInteraction(float deltaTime);

	void SetRotate(float rotation);

	void OnHover();
	void OnPress();
	void OnRelease();

protected:
	glm::vec3 color = glm::vec3(1);
	glm::vec3 defaultColor = glm::vec3(1);
	glm::vec2 spawnPoint = glm::vec2(0);

	Texture* texture = nullptr;
	Mesh* mesh = nullptr;
	Shader* shader = nullptr;

	/// Input vars
	bool isHeld = false;
	bool release = false;
};
