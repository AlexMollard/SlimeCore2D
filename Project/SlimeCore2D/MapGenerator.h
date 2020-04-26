#pragma once
#include "ObjectManager.h"
#include "PhysicsScene.h"

class MapGenerator
{
public:
	enum class type
	{
		Water,
		Ground,
		TopLeft,
		TopCenter,
		TopRight,
		InnerTopLeft,
		InnerTopRight,
		MiddleLeft,
		MiddleRight,
		InnerBottomLeft,
		InnerBottomRight,
		BottomLeft,
		BottomCenter,
		BottomRight,
		Wall,
		Stone,
		Tree
	};

	struct Cell
	{
		glm::vec2 position = glm::vec2(0);
		type cellType = type::Water;
		type preCellType = type::Water;
		GameObject* object = nullptr;
	};

	struct Tree
	{
		glm::vec2 position = glm::vec2(0);
		GameObject* object = nullptr;
	};

	MapGenerator(ObjectManager* objectManager, PhysicsScene* pScene, int mapSize);
	~MapGenerator();

	void Generate();
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

	int GetTotalGroundSurrounding(Cell& cell);
private:
	ObjectManager* objManager;
	PhysicsScene* pScene;

	int mapSize = 0;

	Cell** cells;

	// Textures
	Texture* water = nullptr;

	// Grass
	Texture* center_0 = nullptr;
	Texture* center_1 = nullptr;
	Texture* center_2 = nullptr;
	Texture* center_3 = nullptr;

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
};