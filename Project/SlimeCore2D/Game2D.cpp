#include "Game2D.h"

Game2D::Game2D()
{
	Init();

	map->Generate();

	player = objectManager->CreatePlayer(glm::vec3(0, 0, -0.5f), glm::vec2(1));
	player->Init(camera);
}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;

	delete objectManager;
	objectManager = nullptr;

	delete meshManager;
	meshManager = nullptr;

	delete map;
	map = nullptr;
}

void Game2D::Init()
{
	camera = new Camera(-32, -18, -1, 1);
	meshManager = new MeshManager();
	renderer = new Renderer2D(meshManager, camera);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	map = new MapGenerator(objectManager,75);
}

void Game2D::Update(float deltaTime)
{
	camera->Update(deltaTime);
	objectManager->Update(deltaTime);
}

void Game2D::Draw()
{
	renderer->Draw();
}