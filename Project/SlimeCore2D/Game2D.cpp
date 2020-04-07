#include "Game2D.h"
#define PI 3.1415926535
Game2D::Game2D()
{
	renderer = new Renderer2D();
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	int count = 5;
	// Quad
	for (int i = 1; i < count; i++)
	{
		GameObject* quad = objectManager->CreateQuad(glm::vec2((i - (count * 0.5f)) * 2, -2.5f), glm::vec2(2), glm::vec3(glm::sin((i) * 0.33f), glm::sin((i) * 0.66f), 0.6f));
		physicsScene->addActor(quad, "quad");
	}

	// Circles
	for (int i = 1; i < count; i++)
	{
		GameObject* circle = objectManager->CreateCircle(glm::vec2((i - (count * 0.5f)) * 2, 2.5f), 2, glm::vec3(glm::sin((i) * 0.33f), glm::sin((i) * 0.66f), 0.6f));
		physicsScene->addActor(circle, "circle");
	}

	for ( int i = 0;  i < 360;  i+= 40)
	{
		line = objectManager->CreateLine(15.0f, 0.15f, i, glm::vec3(0.5f, 0.2f, 0.8f));
		physicsScene->addActor(line, "Line", true);
	}

	// Walls
	//------------
	// Bottom
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(0, -36.0f) * 0.5f, glm::vec2(128, 5) * 0.5f, glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Top
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(0, 36.0f) * 0.5f, glm::vec2(128, 5) * 0.5f, glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Right
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(64, 0) * 0.5f, glm::vec2(5, 67) * 0.5f, glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Left
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(-64, 0) * 0.5f, glm::vec2(5, 67) * 0.5f, glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;

	delete objectManager;
	objectManager = nullptr;
}

void Game2D::Update(float deltaTime)
{
	if (inputManager->GetFocus())
	{
		physicsScene->update(deltaTime);
		objectManager->Update(deltaTime);
	}
	//physicsScene->Debug();
}

void Game2D::Draw()
{
	renderer->Draw();
}