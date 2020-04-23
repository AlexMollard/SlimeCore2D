#include "Game2D.h"
#include "gtc/noise.hpp"
#include <time.h>

Game2D::Game2D()
{
	camera = new Camera(-32, -18, -1, 1);
	meshManager = new MeshManager();
	renderer = new Renderer2D(meshManager, camera);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	grass = new Texture("..\\Textures\\Grass\\floor_1.png");
	water = new Texture("..\\Textures\\Water\\WaterOne.png");
	player_Idle = new Texture("..\\Textures\\Player_Idle.png");
	player_Run = new Texture("..\\Textures\\Player_Run.png");

	std::srand(time(0));
	float seed = rand() % 99999999999;
	
	float count = 25;
	float scale = 1.0f;
			
	float amount = (count * scale) * 0.5f;

	player = objectManager->CreateQuad(glm::vec2(0, 0), glm::vec2(scale), player_Idle);
	player->SetFrameRate(0.15f);

	for (float x = -amount; x < amount; x += scale)
	{
		for (float y = -amount; y < amount; y += scale)
		{
			if (y == -amount || y >= amount - scale || x == -amount || x >= amount - scale)
			{
				if (rand()% 2 == 1)
				{
					continue;
				}

				GameObject* quad = objectManager->CreateQuad(glm::vec2(x, y), glm::vec2(scale), water);
				quad->SetFrame(rand() % 5);
				continue;
			}

			GameObject* quad = objectManager->CreateQuad(glm::vec2(x, y), glm::vec2(scale),  grass);
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

	delete player_Idle;
	player_Idle = nullptr;

	delete player_Run;
	player_Run = nullptr;
}

void Game2D::Update(float deltaTime)
{
	camera->Update(deltaTime);


	static float timer;
	timer += deltaTime;
	player->SetPos(camera->GetPosition());

	if (InputManager::GetKeyPress(Keycode::A))
	{
		player->SetScale(glm::vec2(-1, 1));
	}

	if (InputManager::GetKeyPress(Keycode::D))
	{
		player->SetScale(glm::vec2(1, 1));
	}

	if (InputManager::GetKeyPress(Keycode::D) || InputManager::GetKeyPress(Keycode::A) || InputManager::GetKeyPress(Keycode::W) || InputManager::GetKeyPress(Keycode::S))
	{
		player->SetTexture(player_Run);
		player->SetFrameRate(0.1f);
	}
	else
	{
		player->SetTexture(player_Idle);
		player->SetFrameRate(0.15f);
	}

	if (inputManager->GetFocus())
	{
	//	physicsScene->update(deltaTime);
		objectManager->Update(deltaTime);
	}
	//physicsScene->Debug();
}

void Game2D::Draw()
{
	renderer->Draw();
}