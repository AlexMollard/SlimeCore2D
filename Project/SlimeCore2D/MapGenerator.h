#pragma once
#include "ObjectManager.h"
class MapGenerator
{
public:
	enum class type
	{
		None,
		Center,
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
		BottomRight
	};
	struct Cell
	{
		glm::vec2 position = glm::vec2(0);
		type cellType = type::None;
		type preCellType = type::None;
		GameObject* object = nullptr;
	};

	MapGenerator(ObjectManager* objectManager);
	~MapGenerator();

	void Generate(int mapSize);

	type SetType(int x, int y);

	void CreateTextures();
	void DeleteTextures();

	int GetTotalGroundSurrounding(Cell& cell);

	Texture* GetTexture(type tileType);

private:
	ObjectManager* objManager;

	int mapSize = 10;

	Cell** cells;

	// Textures
	Texture* center_0 = nullptr;
	Texture* center_1 = nullptr;
	Texture* center_2 = nullptr;
	Texture* center_3 = nullptr;

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
	Texture* bottom_Right = nullptr;
};

