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
	// m_screenCamera.SetFOV(3.5f);

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
	m_waterBatchRenderer.SetOcclusionCulling(false);

	float miniMapWidth   = 200.0f;
	float miniMapHeight  = 200.0f;
	float miniMapPadding = 20.0f;

	GameObject* miniMapBorderQuad = m_objectManager.CreateQuad(glm::vec3(1920 - miniMapPadding * 0.5f, miniMapPadding * 0.5f, 1), glm::vec2(miniMapWidth + miniMapPadding, miniMapHeight + miniMapPadding));
	miniMapBorderQuad->SetColor(glm::vec3(0.1f,0.1f,0.1f));
	miniMapBorderQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapBorderQuad);

	m_miniMapMask = new Texture();
	//miniMapMask->GenerateRoundedMask(miniMapWidth * 0.45f, miniMapWidth, miniMapHeight);
	m_miniMapMask->GenerateNoise(NoiseType::Perlin, miniMapWidth, miniMapHeight, 0.1, 0.1, 0.1);
	miniMapBorderQuad->SetMaskTexture(m_miniMapMask);


	m_miniMapTexture = m_map->GenerateMiniMap();
	GameObject* miniMapQuad = m_objectManager.CreateQuad(glm::vec3(1920 - miniMapPadding, miniMapPadding, 1), glm::vec2(miniMapWidth, miniMapHeight), m_miniMapTexture);
	miniMapQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapQuad);
	miniMapQuad->SetMaskTexture(m_miniMapMask);

	Texture* fishytex = m_uiBatchRenderer.LoadTexture(ResourceManager::GetTexturePath("fishy"));
	GameObject* fishy = m_objectManager.CreateQuad(glm::vec3(100, 100, 1), glm::vec2(fishytex->GetWidth() * 4, fishytex->GetHeight() * 4), fishytex);
	fishy->SetAnchorPoint({ 0.0f, 0.0f });
	fishy->SetFlipPolicy(FlipPolicy::Vertical);
	m_uiBatchRenderer.AddObject(fishy);

	m_uiBatchRenderer.SetZOffset(-10.0f);
	m_uiBatchRenderer.SetOcclusionCulling(false);
}

Game2D::~Game2D()
{
	delete m_miniMapTexture;
	m_miniMapTexture = nullptr;

	delete m_miniMapMask;
	m_miniMapMask = nullptr;

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
	m_camera.SetPosition({ m_camera.GetPosition().x, m_camera.GetPosition().y - 2.25f });
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_camera.SetPosition({ m_camera.GetPosition().x, m_camera.GetPosition().y + 2.25f });

	m_waterRenderTarget.Unbind();

	RenderTarget::Clear(true, true, { 0.16f, 0.30f, 0.5f, 1.0f });

	m_renderer.Draw(&m_waterBatchRenderer, ShaderType::UI, CameraType::SCREENSPACE, m_time);

	m_renderer.Draw(&m_mapBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_treeBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_cloudBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, sunColour);
	m_renderer.Draw(&m_uiBatchRenderer, ShaderType::UI, CameraType::SCREENSPACE);
}