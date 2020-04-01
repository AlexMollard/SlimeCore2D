#pragma once
#include "Renderer2D.h"
#include "Quad.h"
#include "Circle.h"

class ObjectManager
{
public:
	ObjectManager(Renderer2D* renderer);
	~ObjectManager();

	GameObject* CreateGameObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
	GameObject* CreateQuad(glm::vec2 pos, glm::vec2 size, glm::vec3 color);
	GameObject* CreateCircle(glm::vec2 pos, glm::vec2 diameter, glm::vec3 color);

	void Update(float deltaTime);

protected:
	Renderer2D* renderer = nullptr;
	std::vector<GameObject*> objects;
};

