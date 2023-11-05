#include "pch.h"
#include "Game2D.h"
#include "CloudManager.h"
#include "MapGenerator.h"
#include "engine/ObjectManager.h"
#include "engine/PhysicsScene.h"

Game2D::Game2D()
{
	m_map = new MapGenerator(&m_objectManager, &m_physicsScene, &m_camera, &m_mapBatchRenderer, &m_treeBatchRenderer, 100);
	Input::GetInstance()->SetCamera(&m_camera);

	GameObject* shadow = m_objectManager.CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr);
	shadow->SetHasAnimation(false);

	m_player.Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	m_player.Init(&m_camera, m_map->GetAllCells(), shadow);

	m_map->Generate();

	m_player.SetAllCells(m_map->GetAllCells());
	m_batchRenderer.AddObject(&m_player);

	m_physicsScene.addActor(&m_player, "player");

	m_cloudManager.Init(&m_cloudBatchRenderer, 35);

	Texture* gradient = m_gradientBatchRenderer.LoadTexture(ResourceManager::GetTexturePath("gradient"));
	m_gradientQuad.SetPos(0, 0, -1); // Render in front of everything
	m_gradientQuad.SetTexture(gradient);
	m_gradientBatchRenderer.AddObject(&m_gradientQuad);
}

Game2D::~Game2D()
{
	delete m_map;
	m_map = nullptr;
}

void Game2D::Update(float deltaTime)
{
	m_gradientAmount += deltaTime * 0.5f;
	if (m_gradientAmount > 1.5f)
		m_gradientAmount = 0.0f;

	m_camera.Update(deltaTime);
	m_player.Update(deltaTime);
	m_cloudManager.Update(deltaTime);
	m_physicsScene.update(deltaTime);
	m_objectManager.UpdateFrames(deltaTime);
	m_camera.SetPosition(m_player.GetPos());
	m_map->Update(deltaTime);
}

void Game2D::Draw()
{
	m_renderer.Draw(&m_mapBatchRenderer, ShaderType::BASIC, sunColour);
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, sunColour);
	m_renderer.Draw(&m_treeBatchRenderer, ShaderType::BASIC, sunColour);
	m_renderer.Draw(&m_cloudBatchRenderer, ShaderType::BASIC, sunColour);
	m_renderer.Draw(&m_uiBatchRenderer, ShaderType::UI, sunColour);

	m_renderer.Draw(&m_gradientBatchRenderer, ShaderType::GRADIENT, m_gradientAmount);
}