#pragma once
#include "ObjectManager.h"
#include "PhysicsScene.h"
#include "Debug.h"
#include "MapGenerator.h"
#include "Player.h"
#include "CloudManager.h"
class Game2D
{
public:
	Game2D();
	~Game2D();

	void Init();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* renderer = nullptr;
	ObjectManager* objectManager = nullptr;
	MeshManager* meshManager = nullptr;
	Input* inputManager = Input::GetInstance();
	PhysicsScene* physicsScene = nullptr;
	Camera* camera = nullptr;
	MapGenerator* map = nullptr;
	CloudManager* cloudManager = nullptr;
	GameObject* testObject = nullptr;
	Player* player = nullptr;

	float timer = 0.0f;
};
