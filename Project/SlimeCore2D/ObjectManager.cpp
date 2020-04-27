#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer2D* renderer)
{
	this->renderer = renderer;
}

ObjectManager::~ObjectManager()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->GetIsPlayer() == true)
			delete (Player*)objects[i];
		else
			delete (Quad*)objects[i];

		objects[i] = nullptr;
	}

	delete renderer;
	renderer = nullptr;
}

GameObject* ObjectManager::CreateGameObject(glm::vec3 pos, glm::vec2 size, glm::vec3 color)
{
	GameObject* go = new GameObject();
	go->Create(pos, color, size, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

GameObject* ObjectManager::CreateQuad(glm::vec3 pos, glm::vec2 size, glm::vec3 color)
{
	Quad* go = new Quad();
	go->Create(pos, color, size, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

GameObject* ObjectManager::CreateQuad(glm::vec3 pos, glm::vec2 size, Texture* tex)
{
	Quad* go = new Quad();
	go->Create(pos, glm::vec3(1), size, objects.size());
	go->SetTexture(tex);

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

Player* ObjectManager::CreatePlayer(glm::vec3 pos, glm::vec2 size, glm::vec3 color)
{
	Player* go = new Player();
	go->Create(pos, color, size, objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

void ObjectManager::Update(float deltaTime)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(deltaTime);
		objects[i]->UpdateSpriteTimer(deltaTime);
	}
}

GameObject* ObjectManager::Get(int index)
{
	return objects[index];
}

int ObjectManager::Size()
{
	return objects.size();
}