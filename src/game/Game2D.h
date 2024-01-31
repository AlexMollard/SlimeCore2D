#pragma once
#include "engine/Constants.h"
#include "engine/StateMachine/StateMachine.h"

#include "engine/PhysicsScene.h"
#include "engine/ObjectManager.h"
#include "GameScene.h"
#include "FMODManager.h"

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

	float GetTime() const { return m_time; }

	Camera* GetCamera() { return &m_camera; }
	Camera* GetScreenCamera() { return &m_screenCamera; }

	Renderer2D* GetRenderer() { return &m_renderer; }
	PhysicsScene* GetPhysicsScene() { return &m_physicsScene; }
	ObjectManager* GetObjectManager() { return &m_objectManager; }

private:
	float m_time = 0.0f;

	StateMachine<GameScene> m_stateMachine;

	Camera m_camera = Camera(-16, -9, -1, 1, true);
	Camera m_screenCamera = Camera(16, 9, -10, 10, false);

	Renderer2D m_renderer = Renderer2D(&m_camera, &m_screenCamera);
	PhysicsScene m_physicsScene = PhysicsScene();
	ObjectManager m_objectManager;

	FMODManager* m_manager = nullptr;
};
