#include "MapGenerator.h"
#include "gtc/noise.hpp"
#include <time.h>
#include <iostream>

MapGenerator::MapGenerator(ObjectManager* objectManager, PhysicsScene* pScene,Camera* cam, int mapSize)
{
	objManager = objectManager;
	CreateTextures();
	this->mapSize = mapSize;
	this->pScene = pScene;
	camera = cam;

	// Create Cells
	cells = new Cell * [mapSize];
	for (int i = 0; i < mapSize; i++)
		cells[i] = new Cell[mapSize];


	// Create endResult
	results = new int * [mapSize];
	for (int i = 0; i < mapSize; i++)
		results[i] = new int[mapSize];

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			cells[x][y].cellType = type::Water;

			cells[x][y].object = objManager->CreateQuad(glm::vec3(x - mapSize / 2, y - mapSize / 2, 0), glm::vec2(1));
		}
	}

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

int** MapGenerator::Generate()
{
	int islandSpawnRadius = (mapSize - ((islandRadius * 2) + 2));
	const int islandCount = 55;

	glm::vec2 randomPoint[islandCount];
	randomPoint[0] = glm::vec2(mapSize / 2);

	// Setting random island centers
	for (int i = 1; i < islandCount; i++)
	{
		randomPoint[i] = glm::vec2((rand() % islandSpawnRadius) + islandRadius - 1, (rand() % islandSpawnRadius) + islandRadius - 1);
	}

	// creating islands from centrers
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			type currentType = cells[x][y].cellType;
			for (int i = 0; i < islandCount; i++)
			{
				if (currentType == type::Water)
				{
					float distance = glm::distance(randomPoint[i], glm::vec2(x, y));
					currentType = (distance < (islandRadius - (rand() % (int)islandRadius / 4))) ? type::Ground : type::Water;
				}
				else
					break;
			}

			cells[x][y].cellType = currentType;
			cells[x][y].preCellType = currentType;
			cells[x][y].position = glm::vec2(x, y);
		}
	}

	// Creating Streams/Rivers
	for (int q = 0; q < 15; q++)
	{
		glm::vec2 tile = glm::vec2((rand() % mapSize), (rand() % mapSize));

		while (glm::distance(tile, glm::vec2(mapSize / 2)) < streamWidth * 2)
		{
			tile = glm::vec2((rand() % mapSize), (rand() % mapSize));
		}

		Cell* highestCell = &cells[(int)tile.x][(int)tile.y];

		std::vector<Cell*> streamCells;
		streamCells.push_back(highestCell);
		Cell* nextCell = highestCell;

		while (highestCell->cellType != type::Water && highestCell != nullptr)
		{
			bool right = false;
			bool left = false;
			bool up = false;
			bool down = false;

			highestCell->object->SetColor(glm::vec3(0.1f, 0.3f, 0.75f));
			highestCell->cellType = type::Water;
			// Right
			if (highestCell->position.x < mapSize - 1)
				if (rand() % 4 > 2)
				{
					nextCell = &cells[(int)highestCell->position.x + 1][(int)highestCell->position.y];
					right = true;
				}

			//Left
			if (highestCell->position.x > 0)
				if (rand() % 4 > 2)
				{
					nextCell = &cells[(int)highestCell->position.x - 1][(int)highestCell->position.y];
					left = true;
				}

			// Up
			if (highestCell->position.y < mapSize - 1)
				if (rand() % 4 > 2)
				{
					nextCell = &cells[(int)highestCell->position.x][(int)highestCell->position.y + 1];
					up = true;
				}

			// Down
			if (highestCell->position.y > 0)
				if (rand() % 4 > 2)
				{
					nextCell = &cells[(int)highestCell->position.x][(int)highestCell->position.y - 1];
					down = true;
				}

			if (highestCell == nextCell)
			{
				if (right)
					nextCell = nextCell = &cells[(int)highestCell->position.x + 1][(int)highestCell->position.y];
				else if (left)
					nextCell = nextCell = &cells[(int)highestCell->position.x - 1][(int)highestCell->position.y];
				else if (up)
					nextCell = nextCell = &cells[(int)highestCell->position.x][(int)highestCell->position.y + 1];
				else if (down)
					nextCell = nextCell = &cells[(int)highestCell->position.x][(int)highestCell->position.y - 1];
				else
					break;
			}

			highestCell = nextCell;
			streamCells.push_back(highestCell);
		}

		for (int x = 0; x < mapSize; x++)
		{
			for (int y = 0; y < mapSize; y++)
			{
				for (int i = 0; i < streamCells.size(); i++)
				{
					float distance = glm::distance(streamCells[i]->position, glm::vec2(x, y));
					if (distance < streamWidth)
					{
						cells[x][y].cellType = type::Water;
					}
				}
			}
		}
	}

	// Cleaning up island (turning single water cells to ground cells)
	for (int i = 0; i < 12; i++)
	{
		EatWater(5);
	}

	// Setting all ground tiles next to water into walls
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (GetTotalGroundSurrounding(cells[x][y]) < 8 && cells[x][y].cellType == type::Ground)
				cells[x][y].preCellType = type::Wall;
		}
	}

	SetCellTypeToPreCellType();

	// Setting all tiles to correct sprites and disabling water from rendering
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (GetTotalGroundSurrounding(cells[x][y]) == 0 && cells[x][y].cellType == type::Wall)
			{
				cells[x][y].cellType = type::Water;
			}

			if (cells[x][y].cellType == type::Wall)
			{
				pScene->addActor(cells[x][y].object, "wall", true);
				SetTileSprite(x, y);

				cells[x][y].object->SetColor(glm::vec3(1));
			}

			if (cells[x][y].cellType == type::Water)
			{
				if (rand() % 10 == 2)
				{
					cells[x][y].object->SetColor(glm::vec3(1));
					cells[x][y].object->SetHasAnimation(true);
					cells[x][y].object->SetTexture(water);
					cells[x][y].object->SetFrameRate(0.33f);
					cells[x][y].object->SetFrame(rand() % 3);
					continue;
				}

				cells[x][y].object->SetRender(false);
			}

			if (cells[x][y].cellType == type::Ground)
			{
				cells[x][y].object->SetColor(glm::vec3(1));

				int texValue = rand() % 8;

				// Grass
				if (texValue > 2)	cells[x][y].object->SetTexture(center_0);

				if (texValue == 0)	cells[x][y].object->SetTexture(center_1);

				if (texValue == 1)	cells[x][y].object->SetTexture(center_2);

				if (texValue == 2)	cells[x][y].object->SetTexture(center_3);
			}
		}
	}

	// Assign Stone Tiles
	SetStoneTiles(15);

	// Assign Tree Tiles
	SetTreeTiles(10);

	// Assign sprout Tiles
	SetGrass();

	SetResultValues();

	return results;
}

void MapGenerator::SetCellTypeToPreCellType()
{
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			cells[x][y].cellType = cells[x][y].preCellType;
		}
	}
}

void MapGenerator::EatWater(int landAroundMin)
{
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (GetTotalGroundSurrounding(cells[x][y]) >= landAroundMin && cells[x][y].cellType == type::Water)
			{
				cells[x][y].cellType = type::Ground;
				cells[x][y].object->SetColor(glm::vec3(1.0f));
				cells[x][y].object->SetTexture(center_0);
			}
			cells[x][y].preCellType = cells[x][y].cellType;
		}
	}
}

void MapGenerator::SetStoneTiles(int VainCount)
{
	std::vector<Cell*> stoneMidPoint;
	float maxDistance = 5;

	for (int i = 0; i < VainCount; i++)
	{
		Cell* currentStone = &cells[rand() % mapSize][rand() % mapSize];
	
		while (currentStone->cellType != type::Ground)
		{ 
			currentStone = &cells[rand() % mapSize][rand() % mapSize];
		}
		stoneMidPoint.push_back(currentStone);
	}
	
	// Setting Stone type and Textures
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (cells[x][y].cellType == type::Ground)
			{
				for (int i = 0; i < stoneMidPoint.size(); i++)
				{
					if (glm::distance(cells[x][y].position, stoneMidPoint[i]->position) < (maxDistance - rand() % (int)maxDistance))
					{
						cells[x][y].cellType = type::Stone;
					}
				}
			}

			if (cells[x][y].cellType == type::Stone)
			{
				int texValue = rand() % 8;

				if (texValue > 2)	cells[x][y].object->SetTexture(stone_0);

				if (texValue == 0)	cells[x][y].object->SetTexture(stone_1);

				if (texValue == 1)	cells[x][y].object->SetTexture(stone_2);

				if (texValue == 2)	cells[x][y].object->SetTexture(stone_3);
			}
		}
	}

}

void MapGenerator::SetTreeTiles(int forestCount)
{
	std::vector<Cell*> forsestMidPoint;

	float maxDistance = 10;
	for (int i = 0; i < forestCount; i++)
	{
		Cell* currentStone = &cells[rand() % mapSize][rand() % mapSize];

		while (currentStone->cellType != type::Ground)
		{
			currentStone = &cells[rand() % mapSize][rand() % mapSize];
		}
		forsestMidPoint.push_back(currentStone);
	}

	// Setting Color
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (cells[x][y].cellType == type::Ground)
			{
				for (int i = 0; i < forsestMidPoint.size(); i++)
				{
					if (glm::distance(cells[x][y].position, forsestMidPoint[i]->position) < (maxDistance - rand() % 2))
					{
						cells[x][y].object->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));

						if (rand() % 10 > 8)
						{
							if (treeCell.size() > 0)
							{
								int testing = 0;
								for (int q = 0; q < treeCell.size(); q++)
								{
									if (glm::distance(cells[x][y].position, treeCell[q]->position) > 2)
									{
										testing++;
									}
									else
										break;
								}

								if (testing == treeCell.size())
								{
									glm::vec3 newPos = cells[x][y].object->GetPos();
									newPos = glm::vec3(newPos.x, newPos.y + 1.5f, -0.25f + (y * 0.001f));

									GameObject* tempTree = objManager->CreateQuad(newPos, glm::vec2(1), tree_0);
									tempTree->SetSpriteWidth(32);
									GameObject* shadow = objManager->CreateQuad(glm::vec3(0,0,0.2f), glm::vec2(1), tree_0_Shadow);
									shadow->SetParent(tempTree);
									shadow->UpdatePos();
									shadow->SetSpriteWidth(32);

									treeShadowObjects.push_back(shadow);

									treeCell.push_back(&cells[x][y]);
									trees.push_back(tempTree);
									cells[x][y].cellType = type::Tree;
								}
							}
							else
							{
								glm::vec3 newPos = cells[x][y].object->GetPos();
								newPos = glm::vec3(newPos.x, newPos.y + 1.5f, -0.25f + (y * 0.001f));

								GameObject* tempTree = objManager->CreateQuad(newPos, glm::vec2(1), tree_0);
								tempTree->SetSpriteWidth(32);
								GameObject* shadow = objManager->CreateQuad(glm::vec3(0, 0, 0.2f), glm::vec2(1), tree_0_Shadow);
								shadow->SetParent(tempTree);
								shadow->UpdatePos();
								shadow->SetSpriteWidth(32);

								treeShadowObjects.push_back(shadow);
								treeCell.push_back(&cells[x][y]);
								trees.push_back(tempTree);
									cells[x][y].cellType = type::Tree;
							}
						}
					}
				}
			}
		}
	}
}

type MapGenerator::SetType(int x, int y)
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

	return type::Ground;
}

void MapGenerator::CreateTextures()
{
	water = new Texture("..\\Textures\\Water\\WaterOne.png");

	center_0 = new Texture("..\\Textures\\Grass\\floor_0.png");
	center_1 = new Texture("..\\Textures\\Grass\\floor_1.png");
	center_2 = new Texture("..\\Textures\\Grass\\floor_2.png");
	center_3 = new Texture("..\\Textures\\Grass\\floor_3.png");

	sprout_0 = new Texture("..\\Textures\\Grass\\Sprouts\\sprout_0.png");
	sprout_1 = new Texture("..\\Textures\\Grass\\Sprouts\\sprout_1.png");
	sprout_2 = new Texture("..\\Textures\\Grass\\Sprouts\\sprout_2.png");
	sprout_3 = new Texture("..\\Textures\\Grass\\Sprouts\\sprout_3.png");
	sprout_4 = new Texture("..\\Textures\\Grass\\Sprouts\\sprout_4.png");

	stone_0 = new Texture("..\\Textures\\Stone\\Stone_0.png");
	stone_1 = new Texture("..\\Textures\\Stone\\Stone_1.png");
	stone_2 = new Texture("..\\Textures\\Stone\\Stone_2.png");
	stone_3 = new Texture("..\\Textures\\Stone\\Stone_3.png");

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
	bottom_Center_2 = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Center_2.png");
	bottom_Right = new Texture("..\\Textures\\Ledges\\Wall_Bottom_Right.png");

	tree_0 = new Texture("..\\Textures\\Trees\\tree_0.png");
	tree_0_Shadow = new Texture("..\\Textures\\Trees\\tree_0_shadow.png");
}

void MapGenerator::DeleteTextures()
{
	delete water;
	water = nullptr;

	delete center_0;
	center_0 = nullptr;
	delete center_1;
	center_1 = nullptr;
	delete center_2;
	center_2 = nullptr;
	delete center_3;
	center_3 = nullptr;

	delete sprout_0;
	sprout_0 = nullptr;
	delete sprout_1;
	sprout_1 = nullptr;
	delete sprout_2;
	sprout_2 = nullptr;
	delete sprout_3;
	sprout_3 = nullptr;
	delete sprout_4;
	sprout_4 = nullptr;

	delete stone_0;
	stone_0 = nullptr;
	delete stone_1;
	stone_1 = nullptr;
	delete stone_2;
	stone_2 = nullptr;
	delete stone_3;
	stone_3 = nullptr;

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
	delete bottom_Center_2;
	bottom_Center_2 = nullptr;
	delete bottom_Right;
	bottom_Right = nullptr;

	delete tree_0;
	tree_0 = nullptr;
	delete tree_0_Shadow;
	tree_0_Shadow = nullptr;
}

void MapGenerator::RemakeTerrain()
{
	// Delete the grass and trees
	for (int i = 0; i < trees.size(); i++)
	{
		objManager->RemoveQuad(trees[i]);
		delete trees[i];
		trees[i] = nullptr;
	}
	trees.clear();

	// Delete the grass and trees
	for (int i = 0; i < treeShadowObjects.size(); i++)
	{
		objManager->RemoveQuad(treeShadowObjects[i]);
		delete treeShadowObjects[i];
		treeShadowObjects[i] = nullptr;
	}
	treeShadowObjects.clear();
	treeCell.clear();

	for (int i = 0; i < grassObjects.size(); i++)
	{
		objManager->RemoveQuad(grassObjects[i]);
		delete grassObjects[i];
		grassObjects[i] = nullptr;
	}
	grassObjects.clear();


	// Remove rigid bodies from physics scene
		// Setting all tiles to correct sprites and disabling water from rendering
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (cells[x][y].cellType == type::Wall)
			{
				pScene->removeActor(cells[x][y].object);
			}
			cells[x][y].cellType = type::Water;
			cells[x][y].preCellType = type::Water;
			cells[x][y].object->SetRender(true);
		}
	}

	Generate();
}

void MapGenerator::SetGrass()
{
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			if (cells[x][y].cellType == type::Ground)
			{
				if (rand() % 10 == 1)
				{
					glm::vec3 newPos = cells[x][y].object->GetPos();
					newPos = glm::vec3(newPos.x + ((rand() % 10) - 5) * 0.1f, newPos.y + ((rand() % 10)) * 0.1f, newPos.z - 0.1f);
					Texture* currentTexture = nullptr;

					switch (rand() % 5)
					{
					case 0:
						currentTexture = sprout_0;
						break;
					case 1:
						currentTexture = sprout_1;
						break;
					case 2:
						currentTexture = sprout_2;
						break;
					case 3:
						currentTexture = sprout_3;
						break;
					case 4:
						currentTexture = sprout_4;
						break;
					}

					grassObjects.push_back(objManager->CreateQuad(newPos, glm::vec2(1), currentTexture));
				}
			}
		}
	}
}

void MapGenerator::Update(float deltaTime)
{
	for (int i = 0; i < trees.size(); i++)
	{
			glm::vec3 newPos = trees[i]->GetPos();
		if (trees[i]->GetPos().y - 1 > camera->GetPosition().y)
		{
			trees[i]->SetPos(newPos.x, newPos.y, -0.24f);
		}
		else
		{
			trees[i]->SetPos(newPos.x, newPos.y, -0.6f);
		}
	}
}

Texture* MapGenerator::SetWall(bool up, bool right, bool down, bool left, bool floorAbove, bool floorBelow, bool floorRight, bool floorLeft)
{
	if (down && !up && floorAbove && floorRight)
	{
		return inner_Bottom_Left;
	}

	if (down && !up && floorAbove && floorLeft)
	{
		return inner_Bottom_Right;
	}

	if (up && !down && floorBelow && floorRight)
	{
		return inner_Top_Left;
	}

	if (up && !down && floorBelow && floorLeft)
	{
		return inner_Top_Right;
	}

	if (up && right && !down && !floorBelow && !floorRight)
	{
		return bottom_Left;
	}

	if (up && left && !down && !floorBelow && !floorLeft)
	{
		return bottom_Right;
	}

	if (down && right && !floorAbove && !floorBelow && !floorLeft && !floorRight)
	{
		return top_Left;
	}

	if (down && left && !floorAbove && !floorBelow && !floorLeft && !floorRight)
	{
		return top_Right;
	}


	if (floorBelow && !floorAbove)
	{
		return top_Center;
	}

	if (floorRight && !floorAbove)
	{
		return middle_Left;
	}

	if (floorLeft && !floorAbove)
	{
		return middle_Right;
	}

	return (rand() % 2 == 1) ? bottom_Center : bottom_Center_2;
}

int MapGenerator::GetTotalGroundSurrounding(Cell& cell)
{
	int totalGroundTiles = 0;
	int x = cell.position.x;
	int y = cell.position.y;

	if (x < mapSize - 1)
		if (cells[x + 1][y].cellType != type::Water && cells[x + 1][y].cellType != type::Wall)
			totalGroundTiles++;

	if (x > 0)
		if (cells[x - 1][y].cellType != type::Water && cells[x - 1][y].cellType != type::Wall)
			totalGroundTiles++;

	if (y < mapSize - 1)
	{
		if (cells[x][y + 1].cellType != type::Water && cells[x][y + 1].cellType != type::Wall)
			totalGroundTiles++;

		// Top - Left
		if (x > 0)
			if (cells[x - 1][y + 1].cellType != type::Water && cells[x - 1][y + 1].cellType != type::Wall)
				totalGroundTiles++;

		// Top - Right
		if (x < mapSize - 1)
			if (cells[x + 1][y + 1].cellType != type::Water && cells[x + 1][y + 1].cellType != type::Wall)
				totalGroundTiles++;
	}

	if (y > 0)
	{
		if (cells[x][y - 1].cellType != type::Water && cells[x][y - 1].cellType != type::Wall)
			totalGroundTiles++;

		// Bottom - Left
		if (x > 0)
			if (cells[x - 1][y - 1].cellType != type::Water && cells[x - 1][y - 1].cellType != type::Wall)
				totalGroundTiles++;

		// Bottom - Right
		if (x < mapSize - 1)
			if (cells[x + 1][y - 1].cellType != type::Water && cells[x + 1][y - 1].cellType != type::Wall)
				totalGroundTiles++;
	}

	return totalGroundTiles;
}

void MapGenerator::SetResultValues()
{
	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			switch (cells[x][y].cellType)
			{
			case type::Water:
				results[x][y] = 0;
				break;

			case type::Ground:
				results[x][y] = 1;
				break;

			case type::Stone:
				results[x][y] = 2;
				break;

			case type::Wall:
				results[x][y] = 1;
				break;

			case type::Tree:
				results[x][y] = 3;
				break;

			default:
				results[x][y] = 1;
				break;
			}
		}
	}

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			switch (results[x][y])
			{
			case 0:
				std::cout << "\033[34m" << results[x][y] << "\033[0m";
				break;
			case 1:
				std::cout << "\033[92m" << results[x][y] << "\033[0m";
				break;
			case 2:
				std::cout << "\033[90m" << results[x][y] << "\033[0m";
				break;

			case 3:
				std::cout << "\033[32m" << results[x][y] << "\033[0m";
				break;
			default:
				break;
			}
		}
		std::cout << std::endl;
	}
}

Cell** MapGenerator::GetAllCells()
{
	return cells;
}

Texture* MapGenerator::GetTexture(type tileType)
{
	switch (tileType)
	{
	case type::Water:
		return nullptr;
		break;
	case type::Ground:
		return center_0;
		break;
	case type::TopLeft:
		return top_Left;
		break;
	case type::TopCenter:
		return top_Center;
		break;
	case type::TopRight:
		return top_Right;
		break;
	case type::InnerTopLeft:
		return inner_Top_Left;
		break;
	case type::InnerTopRight:
		return inner_Top_Right;
		break;
	case type::MiddleLeft:
		return middle_Left;
		break;
	case type::MiddleRight:
		return middle_Right;
		break;
	case type::InnerBottomLeft:
		return inner_Bottom_Left;
		break;
	case type::InnerBottomRight:
		return inner_Bottom_Right;
		break;
	case type::BottomLeft:
		return bottom_Left;
		break;
	case type::BottomCenter:
		return bottom_Center;
		break;
	case type::BottomRight:
		return bottom_Right;
		break;
	default:
		break;
	}

	return nullptr;
}

void MapGenerator::SetTileSprite(int x, int y)
{
	bool top = false, right = false, bottom = false, left = false, floorAbove = false, floorBelow = false, floorRight = false, floorLeft = false;

	if (x < mapSize - 1)
	{
		if (cells[x + 1][y].preCellType == type::Wall)
			right = true;

		if (cells[x + 1][y].cellType == type::Ground)
			floorRight = true;
	}

	if (x > 0)
	{
		if (cells[x - 1][y].preCellType == type::Wall)
			left = true;

		if (cells[x - 1][y].cellType == type::Ground)
			floorLeft = true;
	}

	if (y < mapSize - 1)
	{
		if (cells[x][y + 1].preCellType == type::Wall)
			top = true;

		if (cells[x][y + 1].cellType == type::Ground)
			floorAbove = true;
	}

	if (y > 0)
	{
		if (cells[x][y - 1].preCellType == type::Wall)
			bottom = true;

		if (cells[x][y - 1].cellType == type::Ground)
			floorBelow = true;
	}

	cells[x][y].object->SetTexture(SetWall(top, right, bottom, left, floorAbove, floorBelow, floorRight, floorLeft));
}