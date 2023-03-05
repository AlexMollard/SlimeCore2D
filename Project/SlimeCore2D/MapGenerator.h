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
	int mapSize = 0;
	int seed = rand() % 9999999;
	int streamWidth = 5;
	float islandRadius = 10;

	ObjectManager* objManager;
	PhysicsScene* pScene;


	Camera* camera = nullptr;

	Cell** cells;
	int** results;
	std::vector<Cell*> treeCell;
	std::vector<std::shared_ptr<GameObject>> trees;
	std::vector<std::shared_ptr<GameObject>> treeShadowObjects;
	std::vector<std::shared_ptr<GameObject>> grassObjects;

	// Textures
	Texture* water = nullptr;

	// Grass
	Texture* center_0 = nullptr;
	Texture* center_1 = nullptr;
	Texture* center_2 = nullptr;
	Texture* center_3 = nullptr;

	// Sprouts
	Texture* sprout_0 = nullptr;
	Texture* sprout_1 = nullptr;
	Texture* sprout_2 = nullptr;
	Texture* sprout_3 = nullptr;
	Texture* sprout_4 = nullptr;

	// Stone
	Texture* stone_0 = nullptr;
	Texture* stone_1 = nullptr;
	Texture* stone_2 = nullptr;
	Texture* stone_3 = nullptr;

	// Ledges
	Texture* top_Left = nullptr;
	Texture* top_Center = nullptr;
	Texture* top_Right = nullptr;

	Texture* inner_Top_Left = nullptr;
	Texture* inner_Top_Right = nullptr;

	Texture* middle_Left = nullptr;
	Texture* middle_Right = nullptr;

	Texture* inner_Bottom_Left = nullptr;
	Texture* inner_Bottom_Right = nullptr;

	Texture* bottom_Left = nullptr;
	Texture* bottom_Center = nullptr;
	Texture* bottom_Center_2 = nullptr;
	Texture* bottom_Right = nullptr;

	// Trees
	Texture* tree_0 = nullptr;
	Texture* tree_0_Shadow = nullptr;
};