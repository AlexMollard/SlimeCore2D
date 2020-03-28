#include "Game2D.h"

Game2D::Game2D()
{
	renderer->CreateButton(glm::vec3(0, 0, 0), glm::vec3(1),"", glm::vec3(1.5f), 2);
	renderer->CreateQuad(glm::vec3(2.0f, 0, 0), glm::vec3(0.8f, 0.3f, 0.4f));
	renderer->CreateQuad(glm::vec3(-1.25f, -1.5f, 0), glm::vec3(0.3f, 0.4f, 0.8f));

	renderer->CreateButton(glm::vec3(-2.0f, 0.5f, 0), glm::vec3(0.7f, 0.4f, 0.8f));
	renderer->CreateButton(glm::vec3(1.0f, 1.5f, 0), glm::vec3(0.3f, 0.7f, 0.8f));

	renderer->CreateButton(glm::vec3(-1.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-2.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-3.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-4.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-5.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-6.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(0.75f), 3);
	renderer->CreateButton(glm::vec3(-7.25f, 2.5f, 0), glm::vec3(1), "", glm::vec3(1.25f, 2.0f, 1.75f), 3);
}

Game2D::~Game2D()
{
}

void Game2D::Update(float deltaTime)
{
	inputManager->Update();
}

void Game2D::Draw()
{	
	renderer->Draw();
}
