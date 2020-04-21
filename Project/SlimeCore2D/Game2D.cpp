#include "Game2D.h"
#include "gtc/noise.hpp"
#include <time.h>

Game2D::Game2D()
{
	meshManager = new MeshManager();
	renderer = new Renderer2D(meshManager);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	int count = 35;

	std::srand(time(0));
	float seed = rand() % 99999999999;

	// Quad
	for (int x = 0; x < count * 1.5f; x++)
	{
		for (int y = 0; y < count; y++)
		{
			glm::vec3 color = (((glm::simplex(glm::vec2(x + seed, y + seed))) + 1) / 2 > 0.5f) ? glm::vec3(0.2f,0.6f,0.3f) : glm::vec3(0.2f, 0.3f, 0.6f);

			GameObject* quad = objectManager->CreateQuad(glm::vec2(x - ((count * 1.5f) / 2), y - (count / 2)), glm::vec2(1), color);
		}
	}

}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;

	delete objectManager;
	objectManager = nullptr;

	delete meshManager;
	meshManager = nullptr;
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