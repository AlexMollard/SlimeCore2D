#pragma once
#include "CloudManager.h"
#include "MapGenerator.h"
#include "ObjectManager.h"
#include "PhysicsScene.h"
#include "Player.h"
class Game2D
{
public:
	Game2D();
	~Game2D();

	void Init();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* m_renderer         = nullptr;
	ObjectManager* m_objectManager = nullptr;
	Input* m_inputManager          = Input::GetInstance();
	PhysicsScene* m_physicsScene   = nullptr;
	MapGenerator* m_map            = nullptr;
	CloudManager* m_cloudManager   = nullptr;

	Camera m_camera = Camera(-16, -9, -1, 1);
	Player m_player;
};
