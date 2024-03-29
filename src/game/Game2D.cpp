#include "Game2D.h"
#include "engine/MemoryDebugging.h"
#include <engine/ConsoleLog.h>
#include "glew.h"

Game2D::Game2D()
{
	Input::GetInstance()->SetCamera(&m_camera);
	m_screenCamera.UpdateTransformToPutZeroTopLeft();

	GameScene* gameScene = new GameScene();
	m_stateMachine.ChangeState(gameScene, this);

	m_manager = new FMODManager();

}

Game2D::~Game2D()
{
	m_stateMachine.Exit(this);
}

void Game2D::Update(float deltaTime)
{
	m_time += deltaTime;
	m_camera.Update(deltaTime);
	m_physicsScene.update(deltaTime);
	m_objectManager.UpdateFrames(deltaTime);
	m_manager->Update();
	m_stateMachine.Update(this, deltaTime);
}

void Game2D::Draw()
{
	m_stateMachine.Render(this, GetRenderer());
}