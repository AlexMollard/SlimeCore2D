#pragma once
#include "GameObject.h"

struct Cell;
class Texture;
class Camera;

class Player : public GameObject
{
public:
	Player() = default;
	~Player() final;

	void Init(Camera* cam, Cell** cells, GameObject* shadow);

	void Update(float deltaTime);
	void playerMovement(float deltaTime);

	void UpdateSurroundingTiles();
	void SetAllCells(Cell** cells);
private:
	Camera* camera = nullptr;

	Texture* m_playerIdleLeft = nullptr;
	Texture* m_playerIdleRight = nullptr;
	Texture* m_playerRunLeft = nullptr;
	Texture* m_playerRunright = nullptr;
	Texture* m_playerShadow = nullptr;

	Cell** m_cells;
};
