#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer2D* renderer)
{
	this->renderer = renderer;
}

ObjectManager::~ObjectManager()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->GetType() == ObjectType::Quad)
			delete objects[i];
		else
			delete (Quad*)objects[i];

		objects[i] = nullptr;
	}

	delete renderer;
	renderer = nullptr;
}

GameObject* ObjectManager::CreateGameObject(glm::vec2 pos, glm::vec2 size, glm::vec3 color)
{
	GameObject* go = new GameObject(renderer->GetQuadMesh());
	go->Create(pos, color, size, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

GameObject* ObjectManager::CreateQuad(glm::vec2 pos, glm::vec2 size, glm::vec3 color)
{
	Quad* go = new Quad(renderer->GetQuadMesh());
	go->Create(pos, color, size, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

GameObject* ObjectManager::CreateCircle(glm::vec2 pos, glm::vec2 diameter, glm::vec3 color)
{
	Circle* go = new Circle(renderer->GetCircleMesh());
	go->Create(pos, color, diameter, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

void ObjectManager::Update(float deltaTime)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(deltaTime);
	}
}
