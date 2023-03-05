#pragma once
#include "CloudManager.h"
#include "MapGenerator.h"
#include "ObjectManager.h"
#include "PhysicsScene.h"
#include "Player.h"
#include <memory>
class Game2D
{
public:
	Game2D();
	~Game2D();

	void Init();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* renderer         = nullptr;
	ObjectManager* objectManager = nullptr;
	Input* inputManager          = Input::GetInstance();
	PhysicsScene* physicsScene   = nullptr;
	MapGenerator* map            = nullptr;
	CloudManager* cloudManager   = nullptr;

	std::shared_ptr<Camera> camera = nullptr;
	std::shared_ptr<Player> player = nullptr;
};
