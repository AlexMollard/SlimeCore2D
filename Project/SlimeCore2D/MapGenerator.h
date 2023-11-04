#pragma once
#include "ObjectManager.h"
#include "PhysicsScene.h"
#include "Cell.h"
class MapGenerator
{
public:

	MapGenerator(ObjectManager* objectManager, PhysicsScene* pScene,Camera* cam, int mapSize);
	~MapGenerator();

	int** Generate();
	void SetCellTypeToPreCellType();
	void EatWater(int landAroundMin);

	void SetStoneTiles(int VainCount);
	void SetTreeTiles(int forestCount);

	type SetType(int x, int y);
	Texture* SetWall(bool up, bool right, bool down, bool left, bool floorAbove, bool floorBelow, bool floorRight, bool floorLeft);

	Texture* GetTexture(type tileType);
	void SetTileSprite(int x, int y);
	void CreateTextures();
	void DeleteTextures();

	void RemakeTerrain();

	void SetGrass();

	void Update(float deltaTime);

	int GetTotalGroundSurrounding(Cell& cell);

	void SetResultValues();

	Cell** GetAllCells();
private:
	int m_mapSize = 0;
	int m_seed = rand() % 9999999;
	int m_streamWidth = 5;
	float m_islandRadius = 10;

	ObjectManager* m_objManager;
	PhysicsScene* m_physicsScene;

	Camera* m_camera = nullptr;

	Cell** m_cells;
	int** m_results;
	std::vector<Cell*> m_treeCell;
	std::vector<GameObject*> m_trees;
	std::vector<GameObject*> m_treeShadowObjects;
	std::vector<GameObject*> m_grassObjects;

	// Textures
	Texture* m_water = nullptr;

	// Grass
	Texture* m_center0 = nullptr;
	Texture* m_center1 = nullptr;
	Texture* m_center2 = nullptr;
	Texture* m_center3 = nullptr;

	// Sprouts
	Texture* m_sprout0 = nullptr;
	Texture* m_sprout1 = nullptr;
	Texture* m_sprout2 = nullptr;
	Texture* m_sprout3 = nullptr;
	Texture* m_sprout4 = nullptr;

	// Stone
	Texture* m_stone0 = nullptr;
	Texture* m_stone1 = nullptr;
	Texture* m_stone2 = nullptr;
	Texture* m_stone3 = nullptr;

	// Ledges
	Texture* m_topLeft = nullptr;
	Texture* m_topCenter = nullptr;
	Texture* m_topRight = nullptr;

	Texture* m_innerTopLeft = nullptr;
	Texture* m_innerTopRight = nullptr;

	Texture* m_middleLeft = nullptr;
	Texture* m_middleRight = nullptr;

	Texture* m_innerBottomLeft = nullptr;
	Texture* m_innerBottomRight = nullptr;

	Texture* m_bottomLeft = nullptr;
	Texture* m_bottomCenter = nullptr;
	Texture* m_bottomCenter2 = nullptr;
	Texture* m_bottomRight = nullptr;

	// Trees
	Texture* m_tree0 = nullptr;
	Texture* m_tree0Shadow = nullptr;
};