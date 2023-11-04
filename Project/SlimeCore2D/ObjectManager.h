#pragma once
#include "Renderer2D.h"

class GameObject;

class ObjectManager
{
public:
	ObjectManager(Renderer2D* renderer);
	~ObjectManager();

	GameObject* CreateGameObject(glm::vec3 pos, glm::vec2 size, glm::vec3 color);
	GameObject* CreateQuad(glm::vec3 pos, glm::vec2 size = glm::vec2(1), glm::vec3 color = glm::vec3(1));
	GameObject* CreateQuad(glm::vec3 pos, glm::vec2 size, Texture* tex);
	void RemoveQuad(GameObject* object);
	int GetObjectIndex(GameObject* object);

	void Update(float deltaTime);
	void UpdateFrames(float deltaTime);

	GameObject* Get(int index);
	int GetSize();
protected:
	Renderer2D* m_renderer = nullptr;
	std::vector<GameObject*> m_objects;
};
