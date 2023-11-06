#include "pch.h"
#include "Game2D.h"
#include "CloudManager.h"
#include "MapGenerator.h"
#include "engine/ObjectManager.h"
#include "engine/PhysicsScene.h"
#include <engine/Noise.h>

static inline float randFloat()
{
	return rand() / (float)RAND_MAX;
}

Game2D::Game2D()
{
	m_screenCamera.SetPosition(glm::vec2(0.0f, 0.0f));
	m_screenCamera.SetFOV(3.5f);

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

	m_waterTexture = new Texture(m_waterRenderTarget.GetTextureID());
	GameObject* waterQuad = m_objectManager.CreateQuad(glm::vec3(0, 0, 1), glm::vec2(RES_WIDTH, RES_HEIGHT), m_waterTexture);
	waterQuad->SetSpriteWidth(RES_WIDTH);
	waterQuad->SetTextureWidth(RES_WIDTH);
	waterQuad->SetRotation(180.0f);
	waterQuad->SetFlipPolicy(FlipPolicy::Horizontal);
	m_waterBatchRenderer.AddObject(waterQuad);

	GameObject* waterBGQuad = m_objectManager.CreateQuad(glm::vec3(0, 0, 1), glm::vec2(RES_WIDTH, RES_HEIGHT));
	waterBGQuad->SetColor(glm::vec3(0.16f, 0.30f, 0.5f));
	m_waterBGBatchRenderer.AddObject(waterBGQuad);

	// Generate 3 perlin noise textures each zooming in more than the last
	// Using FNL
	// type, width, height, scale, offsetX, offsetY 
	m_perlinNoiseTextures[0].GenerateNoise(NoiseType::Simplex, RES_WIDTH, RES_HEIGHT, 1.01f, randFloat(), randFloat());
	m_perlinNoiseTextures[1].GenerateNoise(NoiseType::Simplex, RES_WIDTH, RES_HEIGHT, 1.05f, randFloat(), randFloat());
	m_perlinNoiseTextures[2].GenerateNoise(NoiseType::Simplex, RES_WIDTH, RES_HEIGHT, 1.1f, randFloat(), randFloat());
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

	// grab the current frame buffer for the water reflection
	m_waterRenderTarget.Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_player.SetPos(glm::vec3(m_player.GetPos().x, m_player.GetPos().y + 2.25f, m_player.GetPos().z));
	m_waterBGBatchRenderer.AddTextureSlot(&m_perlinNoiseTextures[0]);
	m_waterBGBatchRenderer.AddTextureSlot(&m_perlinNoiseTextures[1]);
	m_waterBGBatchRenderer.AddTextureSlot(&m_perlinNoiseTextures[2]);
	m_renderer.Draw(&m_waterBGBatchRenderer, ShaderType::BASIC, CameraType::SCREENSPACE, m_time);
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_player.SetPos(glm::vec3(m_player.GetPos().x, m_player.GetPos().y - 2.25f, m_player.GetPos().z));
	m_waterRenderTarget.Unbind();

	glClearColor(0.16f, 0.30f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_renderer.Draw(&m_waterBatchRenderer, ShaderType::WATER, CameraType::SCREENSPACE, m_time);

	m_renderer.Draw(&m_mapBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_treeBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_cloudBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_uiBatchRenderer, ShaderType::UI, CameraType::ORTHOGRAPHIC);
}