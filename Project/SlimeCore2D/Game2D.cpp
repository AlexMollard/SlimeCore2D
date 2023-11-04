#include "pch.h"
#include "Game2D.h"

Game2D::Game2D()
{
	Init();

	GameObject* shadow = m_objectManager->CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr);
	shadow->SetHasAnimation(false);

	m_player.Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	m_player.Init(&m_camera, m_map->GetAllCells(), shadow);

	m_map->Generate();

	m_player.SetAllCells(m_map->GetAllCells());
	m_renderer->AddObject(&m_player);
	m_physicsScene->addActor(&m_player, "player");

	m_cloudManager->Init(35);
}

Game2D::~Game2D()
{
	delete m_physicsScene;
	m_physicsScene = nullptr;

	delete m_objectManager;
	m_objectManager = nullptr;

	delete m_map;
	m_map = nullptr;

	delete m_cloudManager;
	m_cloudManager = nullptr;
}

void Game2D::Init()
{
	m_renderer      = new Renderer2D(&m_camera);
	m_objectManager = new ObjectManager(m_renderer);
	m_physicsScene  = new PhysicsScene();
	m_map           = new MapGenerator(m_objectManager, m_physicsScene, &m_camera, 100);
	m_cloudManager  = new CloudManager(m_renderer);
	Input::GetInstance()->SetCamera(&m_camera);
}

void Game2D::Update(float deltaTime)
{
	m_camera.Update(deltaTime);
	m_player.Update(deltaTime);
	m_cloudManager->Update(deltaTime);
	m_physicsScene->update(deltaTime);
	m_objectManager->UpdateFrames(deltaTime);
	m_camera.SetPosition(m_player.GetPos());
	m_map->Update(deltaTime);

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
	m_renderer->Draw();
	//physicsScene->Debug();
}