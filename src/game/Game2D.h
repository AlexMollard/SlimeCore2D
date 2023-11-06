#pragma once
#include "Player.h"
#include "engine/BatchRenderer.h"
#include "engine/PhysicsScene.h"
#include "engine/ObjectManager.h"
#include "CloudManager.h"
#include "engine/RenderTarget.h"

class Renderer2D;
class Input;
class MapGenerator;

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

private:
	float m_time = 0.0f;

	glm::vec4 sunColour = glm::vec4(1.0f, 0.7f, 0.8f, 1.0f);

	Camera m_camera = Camera(-16, -9, -10, 10, true);
	Camera m_screenCamera = Camera(-16, -9, -10, 10, false);
	Player m_player;

	Texture m_perlinNoiseTextures[3];

	Renderer2D m_renderer            = Renderer2D(&m_camera, &m_screenCamera);
	RenderTarget m_waterRenderTarget = RenderTarget(RES_WIDTH, RES_HEIGHT, FlipPolicy::Both);

	BatchRenderer m_waterBGBatchRenderer = BatchRenderer();
	BatchRenderer m_waterBatchRenderer = BatchRenderer();
	BatchRenderer m_mapBatchRenderer = BatchRenderer();
	BatchRenderer m_treeBatchRenderer = BatchRenderer();
	BatchRenderer m_cloudBatchRenderer = BatchRenderer();
	BatchRenderer m_batchRenderer = BatchRenderer();
	BatchRenderer m_uiBatchRenderer = BatchRenderer();
	
	PhysicsScene m_physicsScene = PhysicsScene();
	ObjectManager m_objectManager = ObjectManager(&m_renderer);
	CloudManager m_cloudManager = CloudManager();
	MapGenerator* m_map = nullptr;
	Texture* m_waterTexture = nullptr;

	Input* m_inputManager          = Input::GetInstance();
};
