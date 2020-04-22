#include "Game2D.h"
#include "gtc/noise.hpp"
#include <time.h>

Game2D::Game2D()
{
	meshManager = new MeshManager();
	renderer = new Renderer2D(meshManager);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	
	grass = new Texture("..\\Textures\\Grass\\floor_1.png");
	water = new Texture("..\\Textures\\Water\\Water_Sprite_01.png");

	std::srand(time(0));
	float seed = rand() % 99999999999;
	
	float count = 50;
	float scale = 1.0f;
			

	float amount = (count * scale) * 0.5f;

	for (float x = -amount; x < amount; x += scale)
	{
		for (float y = -amount; y < amount; y += scale)
		{
			objectManager->CreateQuad(glm::vec2(x, y), glm::vec2(scale), ((glm::simplex(glm::vec2(x,y)) > -0.25f) ? grass : water));
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

	delete grass;
	grass = nullptr;

	delete water;
	water = nullptr;
}

void Game2D::Update(float deltaTime)
{
	//static float timer;
	//timer += deltaTime;

	//if (inputManager->GetFocus())
	//{
	//	physicsScene->update(deltaTime);
	//	objectManager->Update(deltaTime);
	//}
	//physicsScene->Debug();
}

void Game2D::Draw()
{
	renderer->Draw();
}