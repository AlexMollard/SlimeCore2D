#pragma once
#include "Quad.h"
#include "Camera.h"
#include "Cell.h"

class Player : public Quad
{
public:
	Player();
	~Player();

	void Init(Camera* cam, Cell** cells);

	void Update(float deltaTime);
	void playerMovement(float deltaTime);

	void UpdateSurroundingTiles();
private:
	Camera* camera = nullptr;

	Texture* player_Idle_Left = nullptr;
	Texture* player_Idle_Right = nullptr;
	Texture* player_Run_Left = nullptr;
	Texture* player_Run_Right = nullptr;

	Cell** cells;
};
