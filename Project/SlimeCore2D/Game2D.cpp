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

	bottomLeft = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Left.png");
	bottomCenter = new Texture("..\\Textures\\Ledges\\Bottom_Center_1.png");
	bottomRight = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Right.png");

	centerLeft = new Texture("..\\Textures\\Ledges\\Wall_Left.png");
	centerRight = new Texture("..\\Textures\\Ledges\\Wall_Right.png");

	std::srand(time(0));
	float seed = rand() % 99999999999;
	
	int gridXSize = 20;
	int count = gridXSize * 0.5f;
	float scale = 4.0f;
			
	objectManager->CreateQuad(glm::vec2(scale * -2, 0), glm::vec2(scale), centerLeft);
	objectManager->CreateQuad(glm::vec2(scale * -2, scale * -1), glm::vec2(scale), bottomLeft);
	objectManager->CreateQuad(glm::vec2(scale * -2, scale * -2), glm::vec2(scale), water);

	objectManager->CreateQuad(glm::vec2(scale * -1, 0), glm::vec2(scale), grass);
	objectManager->CreateQuad(glm::vec2(scale * -1, scale * -1), glm::vec2(scale), bottomCenter);
	objectManager->CreateQuad(glm::vec2(scale * -1, scale * -2), glm::vec2(scale), water);
	
	objectManager->CreateQuad(glm::vec2(0, 0), glm::vec2(scale), grass);
	objectManager->CreateQuad(glm::vec2(0, scale * -1), glm::vec2(scale), bottomCenter);

	objectManager->CreateQuad(glm::vec2(scale * 1, 0), glm::vec2(scale), grass);
	objectManager->CreateQuad(glm::vec2(scale * 1, scale * -1), glm::vec2(scale), bottomCenter);
	objectManager->CreateQuad(glm::vec2(scale * 1, scale * -2), glm::vec2(scale), water);

	objectManager->CreateQuad(glm::vec2(scale * 2, 0), glm::vec2(scale), centerRight);
	objectManager->CreateQuad(glm::vec2(scale * 2, scale * -1), glm::vec2(scale), bottomRight);
	objectManager->CreateQuad(glm::vec2(scale * 2, scale * -2), glm::vec2(scale), water);


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

	delete bottomCenter;
	bottomCenter = nullptr;


	delete bottomLeft;
	delete bottomRight;

	delete centerLeft;
	delete centerRight;
}

void Game2D::Update(float deltaTime)
{
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