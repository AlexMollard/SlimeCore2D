#pragma once
#include "Renderer2D.h"
#include <memory>

class ObjectManager
{
public:
	ObjectManager(Renderer2D* renderer);
	~ObjectManager();

	std::shared_ptr<GameObject> CreateGameObject(glm::vec3 pos, glm::vec2 size, glm::vec3 color);
	std::shared_ptr<GameObject> CreateQuad(glm::vec3 pos, glm::vec2 size = glm::vec2(1), glm::vec3 color = glm::vec3(1));
	std::shared_ptr<GameObject> CreateQuad(glm::vec3 pos, glm::vec2 size, Texture* tex);
	void RemoveQuad(std::shared_ptr<GameObject> object);
	int GetObjectIndex(std::shared_ptr<GameObject> object);

	void Update(float deltaTime);
	void UpdateFrames(float deltaTime);

	std::shared_ptr<GameObject> Get(int index);
	int Size();
protected:
	Renderer2D* renderer = nullptr;
	std::vector<std::shared_ptr<GameObject>> objects;
};
