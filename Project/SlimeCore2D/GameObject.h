#pragma once
#include "RigidBody.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class GameObject : public RigidBody
{
public:
	GameObject(Mesh* mesh);
	virtual ~GameObject();

	void Draw();
	virtual void Update(float deltaTime);
	virtual void Create(glm::vec2 pos, glm::vec3 color, glm::vec2 scale, int id);

	Mesh* GetMesh();
	void SetMesh(Mesh* newMesh);

	void Respawn();
	void SetSpawn(glm::vec2 newSpawn);

	glm::vec3 GetColor();
	void SetColor(glm::vec3 newColor);
	void SetColor(float r, float g, float b);

	glm::vec2 GetScale();
	void SetScale(glm::vec2 newScale);

	void SetID(int id);
	int GetID();

	Texture* GetTexture();
	void SetTexture(Texture* tex);

	void UpdateInteraction(float deltaTime);

	void OnHover();
	void OnPress();
	void OnRelease();

protected:
	glm::vec3 color = glm::vec3(1);
	glm::vec3 defaultColor = glm::vec3(1);
	glm::vec2 spawnPoint = glm::vec2(0);

	Texture* texture = nullptr;
	Mesh* mesh = nullptr;

	/// Input vars
	bool isHeld = false;
	bool release = false;
};
