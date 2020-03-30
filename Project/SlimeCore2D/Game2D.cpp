#include "Game2D.h"

Game2D::Game2D()
{
	physicsScene = new PhysicsScene();
	physicsScene->addActor(renderer->CreateButton(glm::vec3(0, 0, 0), glm::vec3(1), "", glm::vec3(1.5f), 1),"Slime");
	physicsScene->addActor(renderer->CreateButton(glm::vec3(2.0f, 0, 0), glm::vec3(0.8f, 0.3f, 0.4f)),"Red");
	physicsScene->addActor(renderer->CreateButton(glm::vec3(-1.25f, -1.5f, 0), glm::vec3(0.3f, 0.4f, 0.8f)),"Blue");
	physicsScene->addActor(renderer->CreateButton(glm::vec3(1.0f, 1.5f, 0), glm::vec3(0.3f, 0.7f, 0.4f),"",glm::vec3(2,1,1)), "Green", true);

	physicsScene->addActor(renderer->CreateQuad(glm::vec3(0, -5, 0), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(25, 1, 1)), "Grey", true);
}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;
}

void Game2D::Update(float deltaTime)
{
	inputManager->Update();
	physicsScene->update(deltaTime);
	renderer->Update(deltaTime);
	physicsScene->Debug();
}

void Game2D::Draw()
{	
	renderer->Draw();
}
