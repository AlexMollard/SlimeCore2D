#include "pch.h"

#include "Game2D.h"

#include "CloudManager.h"
#include "MapGenerator.h"
#include <engine/Noise.h>

#include "engine/ObjectManager.h"
#include "engine/PhysicsScene.h"

static inline float randFloat()
{
	return rand() / (float)RAND_MAX;
}

Game2D::Game2D()
{
	m_screenCamera.UpdateTransformToPutZeroTopLeft();
	//m_screenCamera.SetFOV(3.5f);

	m_map = new MapGenerator(&m_objectManager, &m_physicsScene, &m_camera, &m_mapBatchRenderer, &m_treeBatchRenderer, 100);
	Input::GetInstance()->SetCamera(&m_camera);

	GameObject* shadow = m_objectManager.CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr);
	shadow->SetHasAnimation(false);

	m_player.Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	m_player.Init(&m_camera, m_map->GetAllCells(), shadow);
	m_player.SetHasAnimation(true);

	m_map->Generate();

	m_player.SetAllCells(m_map->GetAllCells());
	m_batchRenderer.AddObject(&m_player);

	m_physicsScene.addActor(&m_player, "player");

	m_cloudManager.Init(&m_cloudBatchRenderer, 35);

	m_waterTexture        = new Texture(m_waterRenderTarget.GetTextureID());
	GameObject* waterQuad = m_objectManager.CreateQuad(glm::vec3(0, -10, 1), glm::vec2(RES_WIDTH, RES_HEIGHT), m_waterTexture);
	waterQuad->SetAnchorPoint({ 0.0f, 0.0f });
	waterQuad->SetSpriteWidth(RES_WIDTH);
	waterQuad->SetScale(glm::vec2(RES_WIDTH, RES_HEIGHT));
	m_waterBatchRenderer.AddObject(waterQuad);

	// Generate 3 perlin noise textures each zooming in more than the last
	m_noiseTextures.GenerateNoise(NoiseType::Simplex, RES_WIDTH, RES_HEIGHT, 0.1f, randFloat(), randFloat());

	GameObject* waterBGQuad = m_objectManager.CreateQuad(glm::vec3(0, 0, 2), glm::vec2(RES_WIDTH, RES_HEIGHT));
	waterBGQuad->SetAnchorPoint({0.0f,0.0f});
	waterBGQuad->SetColor(glm::vec3(0.16f, 0.30f, 0.5f));
	Texture* waterBGtex = m_waterBGBatchRenderer.LoadTexture(ResourceManager::GetTexturePath("Water"));
	waterBGQuad->SetTexture(waterBGtex);
	m_waterBGBatchRenderer.AddObject(waterBGQuad);
}

Game2D::~Game2D()
{
	delete m_waterTexture;
	m_waterTexture = nullptr;

	delete m_map;
	m_map = nullptr;
}

void Game2D::Update(float deltaTime)
{
	m_time += deltaTime;

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

	// Grab the current frame buffer for the water reflection
	m_waterRenderTarget.Bind();
	RenderTarget::Clear(true, true);
	
	// Draw player offset for reflection in water
	m_player.SetPos(glm::vec3(m_player.GetPos().x, m_player.GetPos().y + 2.25f, m_player.GetPos().z));
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_player.SetPos(glm::vec3(m_player.GetPos().x, m_player.GetPos().y - 2.25f, m_player.GetPos().z));
	
	m_waterRenderTarget.Unbind();
	
	RenderTarget::Clear(true, true, { 0.16f, 0.30f, 0.5f, 1.0f });
	
	m_waterBGBatchRenderer.AddTextureSlot(&m_noiseTextures);
	m_renderer.Draw(&m_waterBGBatchRenderer, ShaderType::WATER, CameraType::SCREENSPACE, m_time);

	m_renderer.Draw(&m_waterBatchRenderer, ShaderType::UI, CameraType::SCREENSPACE, m_time);

 	m_renderer.Draw(&m_mapBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
 	m_renderer.Draw(&m_treeBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
 	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
 	m_renderer.Draw(&m_cloudBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
 	m_renderer.Draw(&m_uiBatchRenderer, ShaderType::UI, CameraType::ORTHOGRAPHIC);
}