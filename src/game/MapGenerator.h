#pragma once

#include "Cell.h"
#include <vector>

#include "engine/ObjectManager.h"
#include "engine/PhysicsScene.h"

// Forward declarations
class BatchRenderer;
class Camera;

class MapGenerator
{
public:
	MapGenerator(ObjectManager* objectManager, PhysicsScene* pScene, Camera* cam, BatchRenderer* tileBatch, BatchRenderer* treeBatch, int mapSize);
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
	Texture* GenerateMiniMap();

private:
	int m_mapSize        = 0;
	int m_seed           = rand() % 9999999;
	int m_streamWidth    = 5;
	float m_islandRadius = 10;

	ObjectManager* m_objManager;
	PhysicsScene* m_physicsScene;
	BatchRenderer* m_tileBatch;
	BatchRenderer* m_treeBatch;
	Camera* m_camera;

	Cell** m_cells;
	int** m_results;

	std::vector<Cell*> m_treeCell;
	std::vector<GameObject*> m_trees;
	std::vector<GameObject*> m_treeShadowObjects;
	std::vector<GameObject*> m_grassObjects;

	// Texture Paths
	const char* waterPath         = "/Water/WaterOne";
	const char* centerPaths[4]    = { "/Grass/floor_0", "/Grass/floor_1", "/Grass/floor_2", "/Grass/floor_3" };
	const char* sproutPaths[5]    = { "/Grass/Sprouts/sprout_0", "/Grass/Sprouts/sprout_1", "/Grass/Sprouts/sprout_2", "/Grass/Sprouts/sprout_3", "/Grass/Sprouts/sprout_4" };
	const char* stonePaths[4]     = { "/Stone/Stone_0", "/Stone/Stone_1", "/Stone/Stone_2", "/Stone/Stone_3" };
	const char* topLeftPath       = "/Ledges/Wall_Top_Left";
	const char* topCenterPath     = "/Ledges/Wall_Top";
	const char* topRightPath      = "/Ledges/Wall_Top_Right";
	const char* innerTopLeftPath  = "/Ledges/Wall_Inner_Top_Left";
	const char* innerTopRightPath = "/Ledges/Wall_Inner_Top_Right";
	const char* middleLeftPath    = "/Ledges/Wall_Left";
	const char* middleRightPath   = "/Ledges/Wall_Right";
	const char* innerBottomLeftPath  = "/Ledges/Wall_Inner_Left";
	const char* innerBottomRightPath = "/Ledges/Wall_Inner_Right";
	const char* bottomLeftPath       = "/Ledges/Wall_Bottom_Left";
	const char* bottomCenterPaths[2] = { "/Ledges/Wall_Bottom_Center", "/Ledges/Wall_Bottom_Center_2" };
	const char* bottomRightPath      = "/Ledges/Wall_Bottom_Right";
	const char* treePaths[1]         = { "/Trees/tree_0" };
	const char* treeShadowPath       = "/Trees/tree_0_shadow";

	// Textures
	Texture* m_water;
	std::vector<Texture*> m_center;
	std::vector<Texture*> m_sprout;
	std::vector<Texture*> m_stone;
	Texture* m_topLeft;
	Texture* m_topCenter;
	Texture* m_topRight;
	Texture* m_innerTopLeft;
	Texture* m_innerTopRight;
	Texture* m_middleLeft;
	Texture* m_middleRight;
	Texture* m_innerBottomLeft;
	Texture* m_innerBottomRight;
	Texture* m_bottomLeft;
	std::vector<Texture*> m_bottomCenter;
	Texture* m_bottomRight;
	std::vector<Texture*> m_tree;
	Texture* m_treeShadow;
};
