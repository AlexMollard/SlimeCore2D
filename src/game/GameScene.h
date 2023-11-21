#pragma once
#include "engine/StateMachine/State.h"
#include "Player.h"
#include "engine/QuadBatchRenderer.h"
#include "engine/RenderTarget.h"
#include "CloudManager.h"
#include "engine/Constants.h"

class Game2D;
class MapGenerator;

class GameScene : public State<GameScene>
{
public:
	GameScene()  = default;
	~GameScene() = default;

	void Update(StateMachine<GameScene>* stateMachine, Game2D* game, float dt) override;
	void Enter(StateMachine<GameScene>* stateMachine, Game2D* game) override;
	void Render(StateMachine<GameScene>* stateMachine, Game2D* game, Renderer2D* renderer) override;
	void Exit(StateMachine<GameScene>* stateMachine, Game2D* game) override;

private:
	glm::vec2 m_lightDirection = glm::vec2(1.0f, -1.0f);
	glm::vec4 m_sunColour        = glm::vec4(1.0f, 0.7f, 0.8f, 1.0f);

	Player m_player;

	QuadBatchRenderer m_waterBatchRenderer = QuadBatchRenderer();
	QuadBatchRenderer m_mapBatchRenderer   = QuadBatchRenderer();
	QuadBatchRenderer m_treeBatchRenderer  = QuadBatchRenderer();
	QuadBatchRenderer m_cloudBatchRenderer = QuadBatchRenderer();
	QuadBatchRenderer m_batchRenderer      = QuadBatchRenderer();
	QuadBatchRenderer m_uiBatchRenderer    = QuadBatchRenderer();

	RenderTarget m_waterRenderTarget = RenderTarget(RES_WIDTH, RES_HEIGHT, FlipPolicy::Both);
	Texture* m_waterTexture = nullptr;

	Texture* m_miniMapTexture = nullptr;
	Texture m_noiseTextures;

	MapGenerator* m_map = nullptr;

	CloudManager m_cloudManager   = CloudManager();

	Input* m_inputManager = Input::GetInstance();
};
