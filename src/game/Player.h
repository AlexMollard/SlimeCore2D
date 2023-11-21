#pragma once
#include "engine/GameObject.h"
#include "Bullet.h"
#include "Item.h"

struct Cell;
class Texture;
class Camera;
class QuadBatchRenderer;

class Player final : public GameObject
{
public:
	Player() = default;
	~Player() final;

	void Init(Camera* cam, Cell** cells, GameObject* shadow);
	void SetBatch(QuadBatchRenderer* batch);

	void Update(float deltaTime) override;
	void playerMovement(float deltaTime);

	void UpdateSurroundingTiles();
	void SetAllCells(Cell** cells);

	void useItem(std::shared_ptr<Item> item);

	void PickupItem(std::shared_ptr<Item> item);
	void OnPickupItem(std::shared_ptr<Item> item);

	void shootBullet();
	void OnShootBullet(Bullet* bullet);
	Bullet* GetLastBullet();

private:
	Camera* camera = nullptr;
	QuadBatchRenderer* m_batch = nullptr;

	Texture* m_playerIdleLeft = nullptr;
	Texture* m_playerIdleRight = nullptr;
	Texture* m_playerRunLeft = nullptr;
	Texture* m_playerRunright = nullptr;
	Texture* m_playerShadow = nullptr;

	Cell** m_cells;

	std::vector<Bullet> bullets;
	std::vector<std::shared_ptr<Item>> items;
};
