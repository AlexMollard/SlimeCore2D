#include "Game2D.h"

Game2D::Game2D()
{
	Init();

	map->Generate();

	player = objectManager->CreatePlayer(glm::vec3(0, 0, -0.5f), glm::vec2(1,2));
	player->Init(camera);
	physicsScene->addActor(player, "player");
	//testObject = objectManager->CreateQuad({ Input::GetMouseToWorldPos() , -0.99f }, glm::vec2(1), glm::vec3(1, 1, 1));
	cloudManager->Init(35);
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

	delete cloudManager;
	cloudManager = nullptr;
}

void Game2D::Init()
{
	camera = new Camera(-16, -9, -1, 1);
	meshManager = new MeshManager();
	renderer = new Renderer2D(meshManager, camera);
	objectManager = new ObjectManager(renderer);
	physicsScene = new PhysicsScene();
	map = new MapGenerator(objectManager,physicsScene,75);
	cloudManager = new CloudManager(renderer);
	Input::GetInstance()->SetCamera(camera);
}

void Game2D::Update(float deltaTime)
{
	camera->Update(deltaTime);
	
	physicsScene->update(deltaTime);
	objectManager->Update(deltaTime);
	cloudManager->Update(deltaTime);
}

void Game2D::Draw()
{
	//physicsScene->Debug();
	renderer->Draw();
}