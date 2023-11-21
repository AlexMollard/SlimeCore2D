#include "GameScene.h"

#include "CloudManager.h"
#include "Game2D.h"
#include "MapGenerator.h"

#include "engine/MemoryDebugging.h"
#include "engine/Noise.h"
#include "engine/ResourceManager.h"
#include "engine/StateMachine/StateMachine.h"

void GameScene::Enter(StateMachine<GameScene>* stateMachine, Game2D* game)
{
	Camera* camera                   = game->GetCamera();
	ObjectManager* objectManager     = game->GetObjectManager();
	PhysicsScene* physicsScene       = game->GetPhysicsScene();
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	m_map = new MapGenerator(game->GetObjectManager(), game->GetPhysicsScene(), camera, &m_mapBatchRenderer, &m_treeBatchRenderer, 100);

	GameObject* shadow = objectManager->CreateQuad(glm::vec3(0, 0, 0.3), glm::vec2(1, 2), nullptr);
	shadow->SetHasAnimation(false);

	m_player.Create(glm::vec3(0, 0, -0.5f), glm::vec3(1), glm::vec2(1, 2), 404);
	m_player.Init(camera, m_map->GetAllCells(), shadow);
	m_player.SetHasAnimation(true);
	m_map->Generate();

	m_player.SetAllCells(m_map->GetAllCells());
	m_batchRenderer.AddObject(&m_player);
	m_player.SetBatch(&m_batchRenderer);

	physicsScene->addActor(&m_player, "player");

	m_cloudManager.Init(&m_cloudBatchRenderer, 35);

	m_waterTexture = new Texture(m_waterRenderTarget.GetTextureID());
	resourceManager->AddResource(m_waterTexture, "waterTexture");
	GameObject* waterQuad = objectManager->CreateQuad(glm::vec3(0, -10, 1), glm::vec2(RES_WIDTH, RES_HEIGHT), m_waterTexture);
	waterQuad->SetAnchorPoint({ 0.0f, 0.0f });
	waterQuad->SetSpriteWidth(RES_WIDTH);
	waterQuad->SetScale(glm::vec2(RES_WIDTH, RES_HEIGHT));
	m_waterBatchRenderer.AddObject(waterQuad);
	m_waterBatchRenderer.SetOcclusionCulling(false);

	float miniMapWidth   = 200.0f;
	float miniMapHeight  = 200.0f;
	float miniMapPadding = 20.0f;

	GameObject* miniMapBorderQuad =
	    objectManager->CreateQuad(glm::vec3(1920 - miniMapPadding * 0.5f, miniMapPadding * 0.5f, 1), glm::vec2(miniMapWidth + miniMapPadding, miniMapHeight + miniMapPadding));
	miniMapBorderQuad->SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
	miniMapBorderQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapBorderQuad);

	m_miniMapTexture = m_map->GenerateMiniMap();
	resourceManager->AddResource(m_miniMapTexture, "miniMapTexture");
	GameObject* miniMapQuad = objectManager->CreateQuad(glm::vec3(1920 - miniMapPadding, miniMapPadding, 1), glm::vec2(miniMapWidth, miniMapHeight), m_miniMapTexture);
	miniMapQuad->SetAnchorPoint({ 1.0f, 0.0f });
	m_uiBatchRenderer.AddObject(miniMapQuad);

	Texture* fishytex = resourceManager->LoadTexture(ResourceManager::GetTexturePath("fishy"));
	GameObject* fishy = objectManager->CreateQuad(glm::vec3(100, 100, 1), glm::vec2(fishytex->GetWidth() * 4, fishytex->GetHeight() * 4), fishytex);
	fishy->SetAnchorPoint({ 0.0f, 0.0f });
	fishy->SetFlipPolicy(FlipPolicy::Vertical);
	m_uiBatchRenderer.AddObject(fishy);

	m_uiBatchRenderer.SetZOffset(-10.0f);
	m_uiBatchRenderer.SetOcclusionCulling(false);

	// Example items with custom functions
	Item healthPotion("Health Potion", { 0, 0 }, [](Player& p) { SLIME_INFO("Yeah"); }, nullptr);
	Item speedBoost("Speed Boost", { 1, 0 }, nullptr, [](Player& p) { SLIME_INFO("Nah"); });
	Item damageBoost("Damage Boost", { 2, 0 }, nullptr, [](Player& p) { SLIME_INFO("Pog"); });

	// Register additional event handlers
	healthPotion.registerEventHandler(ItemEvent::OnPickup, [](Player& p) { SLIME_INFO("Health Potion picked up!"); });
	speedBoost.registerEventHandler(ItemEvent::OnShoot, [](Player& p) { SLIME_INFO("Speed Boost activated on shoot!"); });
	damageBoost.registerEventHandler(ItemEvent::OnDamage, [](Player& p) { SLIME_INFO("Damage Boost applied on damage!"); });

	m_player.PickupItem(healthPotion);
	m_player.PickupItem(speedBoost);
	m_player.PickupItem(damageBoost);

	m_player.shootBullet();
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
	delete m_map;
	m_map = nullptr;
}
