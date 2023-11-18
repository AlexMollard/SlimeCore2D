#include "GameScene.h"
#include "engine/MemoryDebugging.h"

#include "Game2D.h"
#include "engine/StateMachine/StateMachine.h"
#include "CloudManager.h"
#include "MapGenerator.h"

#include "engine/Noise.h"
#include "engine/MemoryDebugging.h"
#include "engine/ResourceManager.h"

void GameScene::Enter(StateMachine<GameScene>* stateMachine, Game2D* game)
{
	Camera* camera = game->GetCamera();
	ObjectManager* objectManager = game->GetObjectManager();
	PhysicsScene* physicsScene = game->GetPhysicsScene();

	m_map = new MapGenerator(game->GetObjectManager(), game->GetPhysicsScene(), camera, &m_mapBatchRenderer, &m_treeBatchRenderer, 100);

	GameObject* shadow = objectManager->CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr);
	shadow->SetHasAnimation(false);

	m_player.Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	m_player.Init(camera, m_map->GetAllCells(), shadow);
	m_player.SetHasAnimation(true);

	m_map->Generate();

	m_player.SetAllCells(m_map->GetAllCells());
	m_batchRenderer.AddObject(&m_player);

	physicsScene->addActor(&m_player, "player");

	m_cloudManager.Init(&m_cloudBatchRenderer, 35);

	m_waterTexture        = new Texture(m_waterRenderTarget.GetTextureID());
	GameObject* waterQuad = objectManager->CreateQuad(glm::vec3(0, -10, 1), glm::vec2(RES_WIDTH, RES_HEIGHT), m_waterTexture);
	waterQuad->SetAnchorPoint({ 0.0f, 0.0f });
	waterQuad->SetSpriteWidth(RES_WIDTH);
	waterQuad->SetScale(glm::vec2(RES_WIDTH, RES_HEIGHT));
	m_waterBatchRenderer.AddObject(waterQuad);
	m_waterBatchRenderer.SetOcclusionCulling(false);

	float miniMapWidth   = 200.0f;
	float miniMapHeight  = 200.0f;
	float miniMapPadding = 20.0f;

	GameObject* miniMapBorderQuad = objectManager->CreateQuad(glm::vec3(1920 - miniMapPadding * 0.5f, miniMapPadding * 0.5f, 1),
	                                                           glm::vec2(miniMapWidth + miniMapPadding, miniMapHeight + miniMapPadding));
	miniMapBorderQuad->SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
	miniMapBorderQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapBorderQuad);

	//m_miniMapMask = new Texture();
	//miniMapMask->GenerateRoundedMask(miniMapWidth * 0.45f, miniMapWidth, miniMapHeight);
	//m_miniMapMask->GenerateNoise(NoiseType::Perlin, miniMapWidth, miniMapHeight, 0.1, 0.1, 0.1);
	//miniMapBorderQuad->SetMaskTexture(m_miniMapMask);

	m_miniMapTexture        = m_map->GenerateMiniMap();
	GameObject* miniMapQuad = objectManager->CreateQuad(glm::vec3(1920 - miniMapPadding, miniMapPadding, 1), glm::vec2(miniMapWidth, miniMapHeight), m_miniMapTexture);
	miniMapQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapQuad);
	//miniMapQuad->SetMaskTexture(m_miniMapMask);

	Texture* fishytex = m_uiBatchRenderer.LoadTexture(ResourceManager::GetTexturePath("fishy"));
	GameObject* fishy = objectManager->CreateQuad(glm::vec3(100, 100, 1), glm::vec2(fishytex->GetWidth() * 4, fishytex->GetHeight() * 4), fishytex);
	fishy->SetAnchorPoint({ 0.0f, 0.0f });
	fishy->SetFlipPolicy(FlipPolicy::Vertical);
	m_uiBatchRenderer.AddObject(fishy);

	m_uiBatchRenderer.SetZOffset(-10.0f);
	m_uiBatchRenderer.SetOcclusionCulling(false);
}

void GameScene::Update(StateMachine<GameScene>* stateMachine, Game2D* game, float dt)
{
	m_player.Update(dt);
	m_cloudManager.Update(dt);

	game->GetCamera()->SetPosition(m_player.GetPos());
	m_map->Update(dt);
}

void GameScene::Render(StateMachine<GameScene>* stateMachine, Game2D* game, Renderer2D* renderer)
{
	Camera& camera = *game->GetCamera();

	// Grab the current frame buffer for the water reflection
	m_waterRenderTarget.Bind();
	RenderTarget::Clear(true, true);

	// Draw player offset for reflection in water
	camera.SetPosition({ camera.GetPosition().x, camera.GetPosition().y - 2.25f });
	game->GetRenderer()->Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, m_sunColour);
	camera.SetPosition({ camera.GetPosition().x, camera.GetPosition().y + 2.25f });

	m_waterRenderTarget.Unbind();

	RenderTarget::Clear(true, true, { 0.16f, 0.30f, 0.5f, 1.0f });

	renderer->Draw(&m_waterBatchRenderer, ShaderType::UI, CameraType::SCREENSPACE, game->GetTime());

	renderer->Draw(&m_mapBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, m_sunColour);
	renderer->Draw(&m_treeBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, m_sunColour);
	renderer->Draw(&m_batchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, m_sunColour);
	renderer->Draw(&m_cloudBatchRenderer, ShaderType::BASIC, CameraType::ORTHOGRAPHIC, m_sunColour);
	renderer->Draw(&m_uiBatchRenderer, ShaderType::UI, CameraType::SCREENSPACE);
}

void GameScene::Exit(StateMachine<GameScene>* stateMachine, Game2D* game) 
{
	delete m_miniMapTexture;
	m_miniMapTexture = nullptr;

	//delete m_miniMapMask;
	//m_miniMapMask = nullptr;

	delete m_waterTexture;
	m_waterTexture = nullptr;

	delete m_map;
	m_map = nullptr;
}
