#pragma once
#include "Player.h"
#include "BatchRenderer.h"
#include "engine/PhysicsScene.h"
#include "engine/ObjectManager.h"
#include "CloudManager.h"

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
	float m_gradientAmount = 0.0f;
	bool m_isGradientIncreasing = true;

	glm::vec4 sunColour = glm::vec4(1.0f, 0.7f, 0.8f, 1.0f);

	Camera m_camera = Camera(-16, -9, -10, 10);
	Player m_player;
	GameObject m_gradientQuad;

	Renderer2D m_renderer = Renderer2D(&m_camera);
	BatchRenderer m_mapBatchRenderer = BatchRenderer();
	BatchRenderer m_treeBatchRenderer = BatchRenderer();
	BatchRenderer m_cloudBatchRenderer = BatchRenderer();
	BatchRenderer m_batchRenderer = BatchRenderer();
	BatchRenderer m_gradientBatchRenderer = BatchRenderer();
	BatchRenderer m_uiBatchRenderer = BatchRenderer();
	
	PhysicsScene m_physicsScene = PhysicsScene();
	ObjectManager m_objectManager = ObjectManager(&m_renderer);
	CloudManager m_cloudManager = CloudManager();
	MapGenerator* m_map = nullptr;

	Input* m_inputManager          = Input::GetInstance();
};
