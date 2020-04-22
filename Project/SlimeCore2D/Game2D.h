#pragma once
#include "ObjectManager.h"
#include "PhysicsScene.h"
#include "Debug.h"

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* renderer = nullptr;
	ObjectManager* objectManager = nullptr;
	MeshManager* meshManager = nullptr;
	InputManager* inputManager = InputManager::GetInstance();
	PhysicsScene* physicsScene = nullptr;
	GameObject* line = nullptr;

	Texture* grass = nullptr;
	Texture* water = nullptr;

	//Ledges
	Texture* bottomLeft = nullptr;
	Texture* bottomCenter = nullptr;
	Texture* bottomRight = nullptr;

	Texture* centerLeft = nullptr;
	Texture* centerRight = nullptr;


	float timer = 0.0f;
};
