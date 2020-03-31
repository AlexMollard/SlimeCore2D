#include "Game2D.h"

Game2D::Game2D()
{
	renderer = new Renderer2D();
	physicsScene = new PhysicsScene();

	// Physics Objects
	//------------
	// Slime
	GameObject* slime = renderer->CreateButton(glm::vec3(0, 0, 0), glm::vec3(1), "", glm::vec3(2), 1);
	physicsScene->addActor(slime, "Slime");
	// Red

	GameObject* red = renderer->CreateButton(glm::vec3(2.0f, 0, 0), glm::vec3(0.8f, 0.3f, 0.4f), "", glm::vec3(2));
	physicsScene->addActor(red, "Red");
	
	// Blue
	GameObject* blue = renderer->CreateButton(glm::vec3(-2.25f, -0.5f, 0), glm::vec3(0.3f, 0.4f, 0.8f), "", glm::vec3(2));
	physicsScene->addActor(blue, "Blue");

	// Yellow
	GameObject* yellow = renderer->CreateButton(glm::vec3(-2.25f, 2.5f, 0), glm::vec3(0.8f, 0.7f, 0.2f), "", glm::vec3(2));
	physicsScene->addActor(yellow, "Yellow");

	// Kinematic Objects
	physicsScene->addActor(renderer->CreateButton(glm::vec3(1.0f, 1.5f, 0), glm::vec3(0.3f, 0.7f, 0.4f), "", glm::vec3(5, 1, 1)), "Green", true);

	// Walls
	//------------
	// Bottom
	physicsScene->addActor(renderer->CreateQuad(glm::vec3(0, -18.5f, 0), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(64, 5, 1)), "Grey", true);
	// Top
	physicsScene->addActor(renderer->CreateQuad(glm::vec3(0, 18.5f, 0), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(64, 5, 1)), "Grey", true);
	// Right
	physicsScene->addActor(renderer->CreateQuad(glm::vec3(32, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(5, 32, 1)), "Grey", true);
	// Left
	physicsScene->addActor(renderer->CreateQuad(glm::vec3(-32, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(5, 32, 1)), "Grey", true);

}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;

	delete renderer;
	renderer = nullptr;
}

void Game2D::Update(float deltaTime)
{
	physicsScene->update(deltaTime);
	renderer->Update(deltaTime);
	physicsScene->Debug();
}

void Game2D::Draw()
{	
	renderer->Draw();
}
