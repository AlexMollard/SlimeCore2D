#pragma once
#include "Player.h"
#include "engine/QuadBatchRenderer.h"
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
	glm::vec2 m_lightDirection = glm::vec2(1.0f, -1.0f);

	glm::vec4 sunColour = glm::vec4(1.0f, 0.7f, 0.8f, 1.0f);

	Camera m_camera = Camera(-16, -9, -1, 1, true);
	Camera m_screenCamera = Camera(16, 9, -10, 10, false);
	Player m_player;

	Texture m_noiseTextures;

	Renderer2D m_renderer            = Renderer2D(&m_camera, &m_screenCamera);
	RenderTarget m_waterRenderTarget = RenderTarget(RES_WIDTH, RES_HEIGHT, FlipPolicy::Both);

	QuadBatchRenderer m_waterBatchRenderer   = QuadBatchRenderer();
	QuadBatchRenderer m_mapBatchRenderer     = QuadBatchRenderer();
	QuadBatchRenderer m_treeBatchRenderer    = QuadBatchRenderer();
	QuadBatchRenderer m_cloudBatchRenderer   = QuadBatchRenderer();
	QuadBatchRenderer m_batchRenderer        = QuadBatchRenderer();
	QuadBatchRenderer m_uiBatchRenderer      = QuadBatchRenderer();
	
	PhysicsScene m_physicsScene = PhysicsScene();
	ObjectManager m_objectManager = ObjectManager(&m_renderer);
	CloudManager m_cloudManager = CloudManager();
	MapGenerator* m_map = nullptr;
	Texture* m_waterTexture = nullptr;

	Input* m_inputManager          = Input::GetInstance();
};
