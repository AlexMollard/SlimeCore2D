#include "Game2D.h"

Game2D::Game2D()
{
	Init();

	GameObject* shadow = objectManager->CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr).get();
	shadow->SetHasAnimation(false);

	player = std::make_shared<Player>();
	player->Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	player->Init(camera.get(), map->GetAllCells(), shadow);

	map->Generate();

	player->SetAllCells(map->GetAllCells());
	renderer->AddObject(player);
	physicsScene->addActor(player.get(), "player");

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
}

void Game2D::Init()
{
	camera        = std::make_shared<Camera>(-16, -9, -1, 1);
	renderer      = new Renderer2D(camera);
	objectManager = new ObjectManager(renderer);
	physicsScene  = new PhysicsScene();
	map           = new MapGenerator(objectManager, physicsScene, camera.get(), 100);
	cloudManager  = new CloudManager(renderer);
	Input::GetInstance()->SetCamera(camera.get());
}

void Game2D::Update(float deltaTime)
{
	camera->Update(deltaTime);
	player->Update(deltaTime);
	cloudManager->Update(deltaTime);
	physicsScene->update(deltaTime);
	objectManager->UpdateFrames(deltaTime);
	camera->SetPosition(player->GetPos());
	map->Update(deltaTime);

	static float tempVal = 0.0f;
	tempVal += deltaTime;

	if (tempVal > 0.25f)
	{
		// map->RemakeTerrain();
		tempVal = 0.0f;
	}
}

void Game2D::Draw()
{
	renderer->Draw();
	// physicsScene->Debug();
}