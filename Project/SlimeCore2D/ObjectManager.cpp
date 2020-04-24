#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer2D* renderer)
{
	this->renderer = renderer;
}

ObjectManager::~ObjectManager()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->GetType() == ObjectType::Circle)
			delete (Circle*)objects[i];
		else if (objects[i]->GetType() == ObjectType::Line)
			delete (Line*)objects[i];
		else if (objects[i]->GetIsPlayer() == true)
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

GameObject* ObjectManager::CreateCircle(glm::vec3 pos, float diameter, glm::vec3 color)
{
	Circle* go = new Circle();
	go->Create(pos, color, glm::vec2(diameter), objects.size());

	renderer->AddObject(go);
	objects.push_back(go);

	return go;
}

GameObject* ObjectManager::CreateLine(float distance, float width,float rotation, glm::vec3 color)
{
	Line* go = new Line();

	go->Create(glm::vec3(0), color, glm::vec2(1000, width), objects.size());
	go->SetRotate(rotation);
	renderer->AddObject(go);
	objects.push_back(go);
	go->SetKinematic(true);
	go->SetDistance(distance);

	glm::vec2 normal = go->GetNormal();
	glm::vec2 centre_point = normal * distance;
	go->SetPos(glm::vec3(-centre_point,0));

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
