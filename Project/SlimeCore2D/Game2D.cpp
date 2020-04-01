#include "Game2D.h"

Game2D::Game2D()
{
	renderer = new Renderer2D();
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();

	// Quad
	for (int i = 1; i < 5; i++)
	{
		GameObject* quad = objectManager->CreateQuad(glm::vec2(i * 2, -2.5f), glm::vec2(2), glm::vec3(glm::sin((i) * 0.33f), glm::sin((i) * 0.66f), 0.5f));
		physicsScene->addActor(quad, "quad");
	}

	// Circles
	for (int i = 1; i < 5; i++)
	{
		GameObject* circle = objectManager->CreateCircle(glm::vec2(i * 2, 2.5f), glm::vec2(2), glm::vec3(glm::sin((i) * 0.33f), glm::sin((i) * 0.66f), 0.5f));
		physicsScene->addActor(circle, "circle");
	}


	// Kinematic Objects
	physicsScene->addActor(objectManager->CreateCircle(glm::vec2(1.0f, 1.5f), glm::vec2(3), glm::vec3(0.3f, 0.7f, 0.4f)), "Green",true);

	// Walls
	//------------
	// Bottom
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(0, -18.5f), glm::vec2(64, 5), glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Top
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(0, 18.5f), glm::vec2(64, 5), glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Right
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(32, 0), glm::vec2(5, 32), glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
	// Left
	physicsScene->addActor(objectManager->CreateGameObject(glm::vec2(-32, 0), glm::vec2(5, 32), glm::vec3(0.3f, 0.3f, 0.3f)), "Grey", true);
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
	physicsScene->update(deltaTime);
	objectManager->Update(deltaTime);
	physicsScene->Debug();
}

void Game2D::Draw()
{
	renderer->Draw();
}