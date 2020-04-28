#include "Game2D.h"

Game2D::Game2D()
{
	Init();

	map->Generate();

	player = new Player();
	player->Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2),404);
	player->Init(camera, map->GetAllCells());

	renderer->AddObject(player);
	physicsScene->addActor(player, "player");

	cloudManager->Init(35);
}

Game2D::~Game2D()
{
	delete physicsScene;
	physicsScene = nullptr;

	delete objectManager;
	objectManager = nullptr;

	delete map;
	map = nullptr;

	delete cloudManager;
	cloudManager = nullptr;

	delete player;
	player = nullptr;
}

void Game2D::Init()
{
	camera = new Camera(-16, -9, -1, 1);
	renderer = new Renderer2D(camera);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	map = new MapGenerator(objectManager, physicsScene, 75);
	cloudManager = new CloudManager(renderer);
	Input::GetInstance()->SetCamera(camera);
}

void Game2D::Update(float deltaTime)
{
	camera->Update(deltaTime);
	player->Update(deltaTime);
	cloudManager->Update(deltaTime);
	physicsScene->update(deltaTime);
	camera->SetPosition(player->GetPos());
}

void Game2D::Draw()
{
	renderer->Draw();
}