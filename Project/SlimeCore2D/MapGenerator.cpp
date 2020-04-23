#include "MapGenerator.h"
#include "gtc/noise.hpp"
#include <time.h>

MapGenerator::MapGenerator(ObjectManager* objectManager)
{
	objManager = objectManager;
	CreateTextures();

	srand(time(0));
}

MapGenerator::~MapGenerator()
{
	DeleteTextures();

	for (int i = 0; i < mapSize; i++)
	{
		delete cells[i];
		cells[i] = nullptr;
	}

	delete cells;
	cells = nullptr;
}

void MapGenerator::Generate(int mapSize)
{
	this->mapSize = mapSize;
	int seed = rand() % 9999999;

	// Create Cells
	cells = new Cell * [mapSize];
	for (int i = 0; i < mapSize; i++)
		cells[i] = new Cell[mapSize];


	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			type currentType = ((glm::simplex(glm::vec2(x + seed,y + seed)) + 1) + (glm::simplex(glm::vec2(x + seed * 1.75f, y + seed * 1.75f)) + 1))/ 4 > 0.43f ? type::Center : type::None;

			cells[x][y].cellType = currentType;
			cells[x][y].preCellType = currentType;
			cells[x][y].position = glm::vec2(x, y);
		}
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{		
			int groundTotal = GetTotalGroundSurrounding(cells[x][y]);

			//if (groundTotal < 3 && cells[x][y].cellType == type::Center)
			//{
			//	cells[x][y].preCellType = type::None;
			//}

			if (groundTotal > 7 && cells[x][y].cellType == type::None)
			{
				cells[x][y].preCellType = type::Center;
			}
		}
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			cells[x][y].cellType = cells[x][y].preCellType;

			if (cells[x][y].cellType == type::Center)
			{
				cells[x][y].object = objManager->CreateQuad(glm::vec2(x - mapSize / 2, y - mapSize / 2), glm::vec2(1), GetTexture(cells[x][y].cellType));
				cells[x][y].object->SetColor(glm::vec3(((rand() % 5) * 0.1f) + 0.5f));
			}

		}
	}

}

MapGenerator::type MapGenerator::SetType(int x, int y)
{
	int size = mapSize;

	if (y >= size - 1)
	{
		if (x >= size - 1)
		{
			return type::TopRight;
		}

		if (x <= -size)
		{
			return type::TopLeft;
		}

		return type::TopCenter;
	}

	if (y <= -size)
	{
		if (x >= size - 1)
		{
			return type::BottomRight;
		}

		if (x <= -size)
		{
			return type::BottomLeft;
		}

		return type::BottomCenter;
	}


	if (x >= size - 1)
	{
		return type::MiddleRight;
	}

	if (x <= -size)
	{
		return type::MiddleLeft;
	}


	return type::Center;
}

void MapGenerator::CreateTextures()
{
	center_0 = new Texture("..\\Textures\\Grass\\floor_1.png");
	center_1 = new Texture("..\\Textures\\Grass\\floor_2.png");
	center_2 = new Texture("..\\Textures\\Grass\\floor_3.png");
	center_3 = new Texture("..\\Textures\\Grass\\floor_4.png");

	top_Left = new Texture("..\\Textures\\Ledges\\Wall_Top_Left.png");
	top_Center = new Texture("..\\Textures\\Ledges\\Wall_Top.png");
	top_Right = new Texture("..\\Textures\\Ledges\\Wall_Top_Right.png");

	inner_Top_Left = new Texture("..\\Textures\\Ledges\\Wall_Inner_Top_Left.png");
	inner_Top_Right = new Texture("..\\Textures\\Ledges\\Wall_Inner_Top_Right.png");

	middle_Left = new Texture("..\\Textures\\Ledges\\Wall_Left.png");
	middle_Right = new Texture("..\\Textures\\Ledges\\Wall_Right.png");

	inner_Bottom_Left = new Texture("..\\Textures\\Ledges\\Wall_Inner_Left.png");
	inner_Bottom_Right = new Texture("..\\Textures\\Ledges\\Wall_Inner_Right.png");

	bottom_Left = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Left.png");
	bottom_Center = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Center.png");
	bottom_Right = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Right.png");
}

void MapGenerator::DeleteTextures()
{
	delete center_0;
	center_0 = nullptr;
	delete center_1;
	center_1 = nullptr;
	delete center_2;
	center_2 = nullptr;
	delete center_3;
	center_3 = nullptr;

	delete top_Left;
	top_Left = nullptr;
	delete top_Center;
	top_Center = nullptr;
	delete top_Right;
	top_Right = nullptr;

	delete inner_Top_Left;
	inner_Top_Left = nullptr;
	delete inner_Top_Right;
	inner_Top_Right = nullptr;

	delete middle_Left;
	middle_Left = nullptr;
	delete middle_Right;
	middle_Right = nullptr;

	delete inner_Bottom_Left;
	inner_Bottom_Left = nullptr;
	delete inner_Bottom_Right;
	inner_Bottom_Right = nullptr;

	delete bottom_Left;
	bottom_Left = nullptr;
	delete bottom_Center;
	bottom_Center = nullptr;
	delete bottom_Right;
	bottom_Right = nullptr;
}

int MapGenerator::GetTotalGroundSurrounding(Cell& cell)
{
	int totalGroundTiles = 0;
	int x = cell.position.x;
	int y = cell.position.y;


	if (x < mapSize - 1)
		if (cells[x + 1][ y].cellType != type::None)
			totalGroundTiles++;

	if (x > 0)
		if (cells[x - 1][y].cellType != type::None)
			totalGroundTiles++;

	if (y < mapSize - 1)
	{
		if (cells[x][ y + 1].cellType != type::None)
			totalGroundTiles++;

		// Top - Left
		if (x > 0)
			if (cells[x - 1][ y + 1].cellType != type::None)
				totalGroundTiles++;

		// Top - Right
		if (x < mapSize - 1)
			if (cells[x + 1][ y + 1].cellType != type::None)
				totalGroundTiles++;
	}

	if (y > 0)
	{
		if (cells[x][ y - 1].cellType != type::None)
			totalGroundTiles++;

		// Bottom - Left
		if (x > 0)
			if (cells[x - 1][ y - 1].cellType != type::None)
				totalGroundTiles++;

		// Bottom - Right
		if (x < mapSize - 1)
			if (cells[x + 1][ y - 1].cellType != type::None)
				totalGroundTiles++;
	}

	return totalGroundTiles;

}

Texture* MapGenerator::GetTexture(type tileType)
{
	switch (tileType)
	{
	case MapGenerator::type::None:
		return nullptr;
		break;
	case MapGenerator::type::Center:
		return center_0;
		break;
	case MapGenerator::type::TopLeft:
		return top_Left;
		break;
	case MapGenerator::type::TopCenter:
		return top_Center;
		break;
	case MapGenerator::type::TopRight:
		return top_Right;
		break;
	case MapGenerator::type::InnerTopLeft:
		return inner_Top_Left;
		break;
	case MapGenerator::type::InnerTopRight:
		return inner_Top_Right;
		break;
	case MapGenerator::type::MiddleLeft:
		return middle_Left;
		break;
	case MapGenerator::type::MiddleRight:
		return middle_Right;
		break;
	case MapGenerator::type::InnerBottomLeft:
		return inner_Bottom_Left;
		break;
	case MapGenerator::type::InnerBottomRight:
		return inner_Bottom_Right;
		break;
	case MapGenerator::type::BottomLeft:
		return bottom_Left;
		break;
	case MapGenerator::type::BottomCenter:
		return bottom_Center;
		break;
	case MapGenerator::type::BottomRight:
		return bottom_Right;
		break;
	default:
		break;
	}


	return nullptr;
}
