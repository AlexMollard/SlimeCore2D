#include "Game2D.h"

Game2D::Game2D()
{
	renderer = new Renderer2D();
	inputManager = InputManager::GetInstance();

	renderer->CreateQuad(glm::vec3(0, 0, 0), glm::vec3(0.3f, 0.8f, 0.4f),glm::vec3(1.5f));
	renderer->CreateQuad(glm::vec3(2.0f, 0, 0), glm::vec3(0.8f, 0.3f, 0.4f));
	renderer->CreateQuad(glm::vec3(-1.25f, -1.5f, 0), glm::vec3(0.3f, 0.4f, 0.8f));

	renderer->CreateQuad(glm::vec3(-2.0f, 0.5f, 0), glm::vec3(0.7f, 0.4f, 0.8f));
	renderer->CreateQuad(glm::vec3(1.0f, 1.5f, 0), glm::vec3(0.3f, 0.7f, 0.8f));
}

Game2D::~Game2D()
{
	delete renderer;
	renderer = nullptr;
}

void Game2D::Update(float deltaTime)
{
	inputManager->Update();
}

void Game2D::Draw()
{	
	renderer->Draw();
}
