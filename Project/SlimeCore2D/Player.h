#pragma once
#include "Quad.h"
#include "Camera.h"
#include "Cell.h"

class Player : public Quad
{
public:
	Player();
	~Player();

	void Init(Camera* cam, Cell** cells, GameObject* shadow);

	void Update(float deltaTime);
	void playerMovement(float deltaTime);

	void UpdateSurroundingTiles();
	void SetAllCells(Cell** cells);
private:
	Camera* camera = nullptr;

	Texture* player_Idle_Left = nullptr;
	Texture* player_Idle_Right = nullptr;
	Texture* player_Run_Left = nullptr;
	Texture* player_Run_Right = nullptr;
	Texture* player_Shadow = nullptr;

	Cell** cells;
};
