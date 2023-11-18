
#include "MapGenerator.h"

#include "gtc/noise.hpp"
#include <__msvc_chrono.hpp>
#include <iostream>
#include <random>
#include <time.h>
#include "engine/BatchRenderer.h"
#include "engine/ResourceManager.h"

MapGenerator::MapGenerator(ObjectManager* objectManager, PhysicsScene* pScene, Camera* cam, BatchRenderer* tileBatch, BatchRenderer* treeBatch, int mapSize)
{
	m_objManager = objectManager;
	CreateTextures();
	m_mapSize = mapSize;
	m_physicsScene  = pScene;
	m_camera        = cam;
	m_tileBatch    = tileBatch;
	m_treeBatch    = treeBatch;

	// Create Cells
	m_cells = new Cell*[mapSize];
	for (int i = 0; i < mapSize; i++)
		m_cells[i] = new Cell[mapSize];

	// Create endResult
	m_results = new int*[mapSize];
	for (int i = 0; i < mapSize; i++)
		m_results[i] = new int[mapSize];

	for (int x = 0; x < mapSize; x++)
	{
		for (int y = 0; y < mapSize; y++)
		{
			m_cells[x][y].SetCellType(type::Water);

			m_cells[x][y].SetGameObject(m_objManager->CreateQuad(glm::vec3(x - mapSize / 2, y - mapSize / 2, 0), glm::vec2(1)));
			m_tileBatch->AddObject(m_cells[x][y].GetGameObject());
		}
	}

	srand(time(0));
}

MapGenerator::~MapGenerator()
{
	DeleteTextures();

	for (int i = 0; i < m_mapSize; i++)
	{
		delete m_cells[i];
		m_cells[i] = nullptr;
	}

	delete m_cells;
	m_cells = nullptr;

	for (int x = 0; x < m_mapSize; x++)
	{
		delete m_results[x];
		m_results[x] = nullptr;
	}

	delete m_results;
	m_results = nullptr;
}

int** MapGenerator::Generate()
{
	int islandSpawnRadius = (m_mapSize - ((m_islandRadius * 2) + 2));
	const int islandCount = 55;

	glm::vec2 randomPoint[islandCount];
	randomPoint[0] = glm::vec2(m_mapSize / 2);

	// Setting random island centers
	for (int i = 1; i < islandCount; i++)
	{
		randomPoint[i] = glm::vec2((rand() % islandSpawnRadius) + m_islandRadius - 1, (rand() % islandSpawnRadius) + m_islandRadius - 1);
	}

	// creating islands from centrers
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			type currentType = m_cells[x][y].GetCellType();
			for (int i = 0; i < islandCount; i++)
			{
				if (currentType == type::Water)
				{
					float distance = glm::distance(randomPoint[i], glm::vec2(x, y));
					currentType    = (distance < (m_islandRadius - (rand() % (int)m_islandRadius / 4))) ? type::Ground : type::Water;
				}
				else
					break;
			}

			m_cells[x][y].SetCellType(currentType);
			m_cells[x][y].SetPreCellType(currentType);
			m_cells[x][y].SetPosition(glm::vec2(x, y));
		}
	}

	// Creating Streams/Rivers
	for (int q = 0; q < 15; q++)
	{
		glm::vec2 tile = glm::vec2((rand() % m_mapSize), (rand() % m_mapSize));

		while (glm::distance(tile, glm::vec2(m_mapSize / 2)) < m_streamWidth * 2)
		{
			tile = glm::vec2((rand() % m_mapSize), (rand() % m_mapSize));
		}

		Cell* highestCell = &m_cells[(int)tile.x][(int)tile.y];

		std::vector<Cell*> streamCells;
		streamCells.push_back(highestCell);
		Cell* nextCell = highestCell;

		while (highestCell->GetCellType() != type::Water && highestCell != nullptr)
		{
			bool right = false;
			bool left  = false;
			bool up    = false;
			bool down  = false;

			highestCell->GetGameObject()->SetColor(glm::vec3(0.1f, 0.3f, 0.75f));
			highestCell->SetCellType(type::Water);
			// Right
			if (highestCell->GetPosition().x < m_mapSize - 1)
				if (rand() % 4 > 2)
				{
					nextCell = &m_cells[(int)highestCell->GetPosition().x + 1][(int)highestCell->GetPosition().y];
					right    = true;
				}

			// Left
			if (highestCell->GetPosition().x > 0)
				if (rand() % 4 > 2)
				{
					nextCell = &m_cells[(int)highestCell->GetPosition().x - 1][(int)highestCell->GetPosition().y];
					left     = true;
				}

			// Up
			if (highestCell->GetPosition().y < m_mapSize - 1)
				if (rand() % 4 > 2)
				{
					nextCell = &m_cells[(int)highestCell->GetPosition().x][(int)highestCell->GetPosition().y + 1];
					up       = true;
				}

			// Down
			if (highestCell->GetPosition().y > 0)
				if (rand() % 4 > 2)
				{
					nextCell = &m_cells[(int)highestCell->GetPosition().x][(int)highestCell->GetPosition().y - 1];
					down     = true;
				}

			if (highestCell == nextCell)
			{
				if (right)
					nextCell = nextCell = &m_cells[(int)highestCell->GetPosition().x + 1][(int)highestCell->GetPosition().y];
				else if (left)
					nextCell = nextCell = &m_cells[(int)highestCell->GetPosition().x - 1][(int)highestCell->GetPosition().y];
				else if (up)
					nextCell = nextCell = &m_cells[(int)highestCell->GetPosition().x][(int)highestCell->GetPosition().y + 1];
				else if (down)
					nextCell = nextCell = &m_cells[(int)highestCell->GetPosition().x][(int)highestCell->GetPosition().y - 1];
				else
					break;
			}

			highestCell = nextCell;
			streamCells.push_back(highestCell);
		}

		for (int x = 0; x < m_mapSize; x++)
		{
			for (int y = 0; y < m_mapSize; y++)
			{
				for (int i = 0; i < streamCells.size(); i++)
				{
					float distance = glm::distance(streamCells[i]->GetPosition(), glm::vec2(x, y));
					if (distance < m_streamWidth)
					{
						m_cells[x][y].SetCellType(type::Water);
					}
				}
			}
		}
	}

	// Cleaning up island (turning single water cells to ground cells)
	EatWater(7);

	// Setting all ground tiles next to water into walls
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (GetTotalGroundSurrounding(m_cells[x][y]) < 8 && m_cells[x][y].GetCellType() == type::Ground)
				m_cells[x][y].SetPreCellType(type::Wall);
		}
	}

	SetCellTypeToPreCellType();

	// Setting all tiles to correct sprites and disabling water from rendering
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (GetTotalGroundSurrounding(m_cells[x][y]) == 0 && m_cells[x][y].GetCellType() == type::Wall)
			{
				m_cells[x][y].SetCellType(type::Water);
			}

			if (m_cells[x][y].GetCellType() == type::Wall)
			{
				m_physicsScene->addActor(m_cells[x][y].GetGameObject(), "wall", true);
				SetTileSprite(x, y);

				m_cells[x][y].GetGameObject()->SetColor(glm::vec3(1));
			}

			if (m_cells[x][y].GetCellType() == type::Water)
			{
				if (rand() % 10 == 2)
				{
					m_cells[x][y].GetGameObject()->SetColor(glm::vec3(1));
					m_cells[x][y].GetGameObject()->SetHasAnimation(true);
					m_cells[x][y].GetGameObject()->SetTexture(m_water);
					m_cells[x][y].GetGameObject()->SetFrameRate(0.33f);
					m_cells[x][y].GetGameObject()->SetFrame(rand() % 3);
					continue;
				}

				m_cells[x][y].GetGameObject()->SetRender(false);
			}

			if (m_cells[x][y].GetCellType() == type::Ground)
			{
				m_cells[x][y].GetGameObject()->SetColor(glm::vec3(1));

				int texValue = rand() % 8;

				// Grass
				if (texValue > 2)
					m_cells[x][y].GetGameObject()->SetTexture(m_center0);

				if (texValue == 0)
					m_cells[x][y].GetGameObject()->SetTexture(m_center1);

				if (texValue == 1)
					m_cells[x][y].GetGameObject()->SetTexture(m_center2);

				if (texValue == 2)
					m_cells[x][y].GetGameObject()->SetTexture(m_center3);
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

	return m_results;
}

void MapGenerator::SetCellTypeToPreCellType()
{
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			m_cells[x][y].SetCellType(m_cells[x][y].GetPreCellType());
		}
	}
}

void MapGenerator::EatWater(int landAroundMin)
{
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (GetTotalGroundSurrounding(m_cells[x][y]) >= landAroundMin && m_cells[x][y].GetCellType() == type::Water)
			{
				m_cells[x][y].SetCellType(type::Ground);
				m_cells[x][y].GetGameObject()->SetColor(glm::vec3(1.0f));
				m_cells[x][y].GetGameObject()->SetTexture(m_center0);
			}
			m_cells[x][y].SetPreCellType(m_cells[x][y].GetCellType());
		}
	}
}

void MapGenerator::SetStoneTiles(int VainCount)
{
	std::vector<Cell*> stoneMidPoint;
	float maxDistance = 5;

	for (int i = 0; i < VainCount; i++)
	{
		Cell* currentStone = &m_cells[rand() % m_mapSize][rand() % m_mapSize];

		while (currentStone->GetCellType() != type::Ground)
		{
			currentStone = &m_cells[rand() % m_mapSize][rand() % m_mapSize];
		}
		stoneMidPoint.push_back(currentStone);
	}

	// Setting Stone type and Textures
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (m_cells[x][y].GetCellType() == type::Ground)
			{
				for (int i = 0; i < stoneMidPoint.size(); i++)
				{
					if (glm::distance(m_cells[x][y].GetPosition(), stoneMidPoint[i]->GetPosition()) < (maxDistance - rand() % (int)maxDistance))
					{
						m_cells[x][y].SetCellType(type::Stone);
					}
				}
			}

			if (m_cells[x][y].GetCellType() == type::Stone)
			{
				int texValue = rand() % 8;

				if (texValue > 2)
					m_cells[x][y].GetGameObject()->SetTexture(m_stone0);

				if (texValue == 0)
					m_cells[x][y].GetGameObject()->SetTexture(m_stone1);

				if (texValue == 1)
					m_cells[x][y].GetGameObject()->SetTexture(m_stone2);

				if (texValue == 2)
					m_cells[x][y].GetGameObject()->SetTexture(m_stone3);
			}
		}
	}
}

void MapGenerator::SetTreeTiles(int forestCount)
{
	// Generate midpoints for forests
	std::vector<Cell*> forestMidpoints;
	const float maxDistance = 10;

	std::uniform_int_distribution dist(0, m_mapSize - 1);
	std::default_random_engine rng;
	rng.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	auto isGround    = [](const Cell& cell) { return cell.GetCellType() == type::Ground; };
	auto isNotGround = [](const Cell& cell) { return cell.GetCellType() != type::Ground; };

	while (forestMidpoints.size() < forestCount)
	{
		int x = dist(rng);
		int y = dist(rng);
		if (isGround(m_cells[x][y]) &&
		    std::ranges::none_of(forestMidpoints.begin(), forestMidpoints.end(),
		        [&, x, y, maxDistance](const Cell* midpoint) 
				{
					return glm::distance(m_cells[x][y].GetPosition(), midpoint->GetPosition()) < (maxDistance - rand() % 2); 
				}))
		{
			forestMidpoints.push_back(&m_cells[x][y]);
		}
	}

	// Setting Color
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			auto& cell = m_cells[x][y];
			if (cell.GetCellType() != type::Ground)
			{
				continue;
			}

			for (auto& midPoint : forestMidpoints)
			{
				if (glm::distance(cell.GetPosition(), midPoint->GetPosition()) < (maxDistance - rand() % 2))
				{
					cell.GetGameObject()->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));

					if (rand() % 10 > 8)
					{
						if (!m_treeCell.empty() &&
						    std::ranges::all_of(m_treeCell.begin(), m_treeCell.end(), [&](const auto& tree) { return glm::distance(cell.GetPosition(), tree->GetPosition()) > 2; }))
						{
							glm::vec3 newPos = cell.GetGameObject()->GetPos();
							newPos           = glm::vec3(newPos.x, newPos.y + 1.5f, -0.25f + (y * 0.001f));

							auto tempTree = m_objManager->CreateQuad(newPos, glm::vec2(1), m_tree0);
							tempTree->SetSpriteWidth(32);
							tempTree->SetScale({ 2, 4 });
							auto shadow = m_objManager->CreateQuad(glm::vec3(0, 0, 0.2f), glm::vec2(1), m_tree0Shadow);
							shadow->SetParent(tempTree);
							shadow->UpdatePos();
							shadow->SetSpriteWidth(32);
							shadow->SetScale({ 2, 4 });

							m_treeShadowObjects.push_back(shadow);

							m_treeCell.push_back(&cell);
							m_trees.push_back(tempTree);
							cell.SetCellType(type::Tree);

							m_treeBatch->AddObject(tempTree);
							m_treeBatch->AddObject(shadow);
						}
						else if (m_treeCell.empty())
						{
							glm::vec3 newPos = cell.GetGameObject()->GetPos();
							newPos           = glm::vec3(newPos.x, newPos.y + 1.5f, -0.25f + (y * 0.001f));

							auto tempTree = m_objManager->CreateQuad(newPos, glm::vec2(1), m_tree0);
							tempTree->SetSpriteWidth(32);
							tempTree->SetScale({ 2, 4 });
							auto shadow = m_objManager->CreateQuad(glm::vec3(0, 0, 0.2f), glm::vec2(1), m_tree0Shadow);
							shadow->SetParent(tempTree);
							shadow->UpdatePos();
							shadow->SetSpriteWidth(32);
							shadow->SetScale({ 2, 4 });


							m_treeShadowObjects.push_back(shadow);
							m_treeCell.push_back(&cell);
							m_trees.push_back(tempTree);
							cell.SetCellType(type::Tree);

							m_treeBatch->AddObject(tempTree);
							m_treeBatch->AddObject(shadow);
						}
					}
				}
			}
		}
	}
}

type MapGenerator::SetType(int x, int y)
{
	int size = m_mapSize;

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
	m_water = new Texture(ResourceManager::GetTexturePath("/Water/WaterOne"));

	m_center0 = new Texture(ResourceManager::GetTexturePath("/Grass/floor_0"));
	m_center1 = new Texture(ResourceManager::GetTexturePath("/Grass/floor_1"));
	m_center2 = new Texture(ResourceManager::GetTexturePath("/Grass/floor_2"));
	m_center3 = new Texture(ResourceManager::GetTexturePath("/Grass/floor_3"));

	m_sprout0 = new Texture(ResourceManager::GetTexturePath("/Grass/Sprouts/sprout_0"));
	m_sprout1 = new Texture(ResourceManager::GetTexturePath("/Grass/Sprouts/sprout_1"));
	m_sprout2 = new Texture(ResourceManager::GetTexturePath("/Grass/Sprouts/sprout_2"));
	m_sprout3 = new Texture(ResourceManager::GetTexturePath("/Grass/Sprouts/sprout_3"));
	m_sprout4 = new Texture(ResourceManager::GetTexturePath("/Grass/Sprouts/sprout_4"));

	m_stone0 = new Texture(ResourceManager::GetTexturePath("/Stone/Stone_0"));
	m_stone1 = new Texture(ResourceManager::GetTexturePath("/Stone/Stone_1"));
	m_stone2 = new Texture(ResourceManager::GetTexturePath("/Stone/Stone_2"));
	m_stone3 = new Texture(ResourceManager::GetTexturePath("/Stone/Stone_3"));

	m_topLeft   = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Top_Left"));
	m_topCenter = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Top"));
	m_topRight  = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Top_Right"));

	m_innerTopLeft  = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Inner_Top_Left"));
	m_innerTopRight = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Inner_Top_Right"));

	m_middleLeft  = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Left"));
	m_middleRight = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Right"));

	m_innerBottomLeft  = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Inner_Left"));
	m_innerBottomRight = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Inner_Right"));

	m_bottomLeft     = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Bottom_Left"));
	m_bottomCenter   = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Bottom_Center"));
	m_bottomCenter2 =  new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Bottom_Center_2"));
	m_bottomRight = new Texture(ResourceManager::GetTexturePath("/Ledges/Wall_Bottom_Right"));

	m_tree0       = new Texture(ResourceManager::GetTexturePath("/Trees/tree_0"));
	m_tree0Shadow = new Texture(ResourceManager::GetTexturePath("/Trees/tree_0_shadow"));
}

void MapGenerator::DeleteTextures()
{
	delete m_water;
	m_water = nullptr;

	delete m_center0;
	m_center0 = nullptr;
	delete m_center1;
	m_center1 = nullptr;
	delete m_center2;
	m_center2 = nullptr;
	delete m_center3;
	m_center3 = nullptr;

	delete m_sprout0;
	m_sprout0 = nullptr;
	delete m_sprout1;
	m_sprout1 = nullptr;
	delete m_sprout2;
	m_sprout2 = nullptr;
	delete m_sprout3;
	m_sprout3 = nullptr;
	delete m_sprout4;
	m_sprout4 = nullptr;

	delete m_stone0;
	m_stone0 = nullptr;
	delete m_stone1;
	m_stone1 = nullptr;
	delete m_stone2;
	m_stone2 = nullptr;
	delete m_stone3;
	m_stone3 = nullptr;

	delete m_topLeft;
	m_topLeft = nullptr;
	delete m_topCenter;
	m_topCenter = nullptr;
	delete m_topRight;
	m_topRight = nullptr;

	delete m_innerTopLeft;
	m_innerTopLeft = nullptr;
	delete m_innerTopRight;
	m_innerTopRight = nullptr;

	delete m_middleLeft;
	m_middleLeft = nullptr;
	delete m_middleRight;
	m_middleRight = nullptr;

	delete m_innerBottomLeft;
	m_innerBottomLeft = nullptr;
	delete m_innerBottomRight;
	m_innerBottomRight = nullptr;

	delete m_bottomLeft;
	m_bottomLeft = nullptr;
	delete m_bottomCenter;
	m_bottomCenter = nullptr;
	delete m_bottomCenter2;
	m_bottomCenter2 = nullptr;
	delete m_bottomRight;
	m_bottomRight = nullptr;

	delete m_tree0;
	m_tree0 = nullptr;
	delete m_tree0Shadow;
	m_tree0Shadow = nullptr;
}

void MapGenerator::RemakeTerrain()
{
	// Remove trees
	for (auto& tree : m_trees)
	{
		m_objManager->RemoveQuad(tree);
		delete tree;
		tree = nullptr;
	}
	m_trees.clear();

	// Remove tree shadows
	for (auto& shadow : m_treeShadowObjects)
	{
		m_objManager->RemoveQuad(shadow);
		delete shadow;
		shadow = nullptr;
	}
	m_treeShadowObjects.clear();
	m_treeCell.clear();

	// Remove grass
	for (auto& grass : m_grassObjects)
	{
		m_objManager->RemoveQuad(grass);
		delete grass;
		grass = nullptr;
	}
	m_grassObjects.clear();

	// Remove wall tiles and reset other tiles
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (m_cells[x][y].GetCellType() == type::Wall)
			{
				m_physicsScene->removeActor(m_cells[x][y].GetGameObject());
			}
			m_cells[x][y].SetCellType(type::Water);
			m_cells[x][y].SetPreCellType(type::Water);
			m_cells[x][y].GetGameObject()->SetRender(true);
		}
	}

	Generate();
}

void MapGenerator::SetGrass()
{
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			if (m_cells[x][y].GetCellType() == type::Ground)
			{
				if (rand() % 10 == 1)
				{
					glm::vec3 newPos        = m_cells[x][y].GetGameObject()->GetPos();
					newPos                  = glm::vec3(newPos.x + ((rand() % 10) - 5) * 0.1f, newPos.y + ((rand() % 10)) * 0.1f, newPos.z - 0.1f);
					Texture* currentTexture = nullptr;

					switch (rand() % 5)
					{
					case 0: currentTexture = m_sprout0; break;
					case 1: currentTexture = m_sprout1; break;
					case 2: currentTexture = m_sprout2; break;
					case 3: currentTexture = m_sprout3; break;
					case 4: currentTexture = m_sprout4; break;
					}

					m_grassObjects.push_back(m_objManager->CreateQuad(newPos, glm::vec2(1), currentTexture));
				}
			}
		}
	}
}

void MapGenerator::Update(float deltaTime)
{
	for (int i = 0; i < m_trees.size(); i++)
	{
		glm::vec3 newPos = m_trees[i]->GetPos();
		if (m_trees[i]->GetPos().y - 1 > m_camera->GetPosition().y)
		{
			m_trees[i]->SetPos(newPos.x, newPos.y, -0.24f);
		}
		else
		{
			m_trees[i]->SetPos(newPos.x, newPos.y, -0.6f);
		}
	}
}

Texture* MapGenerator::SetWall(bool up, bool right, bool down, bool left, bool floorAbove, bool floorBelow, bool floorRight, bool floorLeft)
{
	if (down && !up && floorAbove && floorRight)
	{
		return m_innerBottomLeft;
	}

	if (down && !up && floorAbove && floorLeft)
	{
		return m_innerBottomRight;
	}

	if (up && !down && floorBelow && floorRight)
	{
		return m_innerTopLeft;
	}

	if (up && !down && floorBelow && floorLeft)
	{
		return m_innerTopRight;
	}

	if (up && right && !down && !floorBelow && !floorRight)
	{
		return m_bottomLeft;
	}

	if (up && left && !down && !floorBelow && !floorLeft)
	{
		return m_bottomRight;
	}

	if (down && right && !floorAbove && !floorBelow && !floorLeft && !floorRight)
	{
		return m_topLeft;
	}

	if (down && left && !floorAbove && !floorBelow && !floorLeft && !floorRight)
	{
		return m_topRight;
	}

	if (floorBelow && !floorAbove)
	{
		return m_topCenter;
	}

	if (floorRight && !floorAbove)
	{
		return m_middleLeft;
	}

	if (floorLeft && !floorAbove)
	{
		return m_middleRight;
	}

	return (rand() % 2 == 1) ? m_bottomCenter : m_bottomCenter2;
}

int MapGenerator::GetTotalGroundSurrounding(Cell& cell)
{
	int totalGroundTiles = 0;
	int x                = cell.GetPosition().x;
	int y                = cell.GetPosition().y;

	if (x < m_mapSize - 1)
		if (m_cells[x + 1][y].GetCellType() != type::Water && m_cells[x + 1][y].GetCellType() != type::Wall)
			totalGroundTiles++;

	if (x > 0)
		if (m_cells[x - 1][y].GetCellType() != type::Water && m_cells[x - 1][y].GetCellType() != type::Wall)
			totalGroundTiles++;

	if (y < m_mapSize - 1)
	{
		if (m_cells[x][y + 1].GetCellType() != type::Water && m_cells[x][y + 1].GetCellType() != type::Wall)
			totalGroundTiles++;

		// Top - Left
		if (x > 0)
			if (m_cells[x - 1][y + 1].GetCellType() != type::Water && m_cells[x - 1][y + 1].GetCellType() != type::Wall)
				totalGroundTiles++;

		// Top - Right
		if (x < m_mapSize - 1)
			if (m_cells[x + 1][y + 1].GetCellType() != type::Water && m_cells[x + 1][y + 1].GetCellType() != type::Wall)
				totalGroundTiles++;
	}

	if (y > 0)
	{
		if (m_cells[x][y - 1].GetCellType() != type::Water && m_cells[x][y - 1].GetCellType() != type::Wall)
			totalGroundTiles++;

		// Bottom - Left
		if (x > 0)
			if (m_cells[x - 1][y - 1].GetCellType() != type::Water && m_cells[x - 1][y - 1].GetCellType() != type::Wall)
				totalGroundTiles++;

		// Bottom - Right
		if (x < m_mapSize - 1)
			if (m_cells[x + 1][y - 1].GetCellType() != type::Water && m_cells[x + 1][y - 1].GetCellType() != type::Wall)
				totalGroundTiles++;
	}

	return totalGroundTiles;
}

void MapGenerator::SetResultValues()
{
	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			switch (m_cells[x][y].GetCellType())
			{
			case type::Water: m_results[x][y] = 0; break;

			case type::Ground: m_results[x][y] = 1; break;

			case type::Stone: m_results[x][y] = 2; break;

			case type::Wall: m_results[x][y] = 1; break;

			case type::Tree: m_results[x][y] = 3; break;

			default: m_results[x][y] = 1; break;
			}
		}
	}

	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			switch (m_results[x][y])
			{
			case 0: std::cout << "\033[34m" << m_results[x][y] << "\033[0m"; break;
			case 1: std::cout << "\033[92m" << m_results[x][y] << "\033[0m"; break;
			case 2: std::cout << "\033[90m" << m_results[x][y] << "\033[0m"; break;

			case 3: std::cout << "\033[32m" << m_results[x][y] << "\033[0m"; break;
			default: break;
			}
		}
		std::cout << std::endl;
	}
}

Cell** MapGenerator::GetAllCells()
{
	return m_cells;
}

Texture* MapGenerator::GenerateMiniMap()
{
	float* miniMap = new float[m_mapSize * m_mapSize * 4]; // 4 = RGBA

	for (int x = 0; x < m_mapSize; x++)
	{
		for (int y = 0; y < m_mapSize; y++)
		{
			int index = (x + y * m_mapSize) * 4;

			switch (m_cells[x][y].GetCellType())
			{
			case type::Water:
				miniMap[index + 0] = 0.2f;
				miniMap[index + 1] = 0.3f;
				miniMap[index + 2] = 0.7f;
				miniMap[index + 3] = 1.0f;
				break;

			case type::Ground:
				miniMap[index + 0] = 0.2f;
				miniMap[index + 1] = 0.7f;
				miniMap[index + 2] = 0.3f;
				miniMap[index + 3] = 1.0f;
				break;

			case type::Stone:
				miniMap[index + 0] = 0.5f;
				miniMap[index + 1] = 0.5f;
				miniMap[index + 2] = 0.5f;
				miniMap[index + 3] = 1.0f;
				break;

			case type::Wall:
				miniMap[index + 0] = 0.1f;
				miniMap[index + 1] = 0.35f;
				miniMap[index + 2] = 0.15f;
				miniMap[index + 3] = 1.0f;
				break;

			case type::Tree:
				miniMap[index + 0] = 0.1f;
				miniMap[index + 1] = 0.5f;
				miniMap[index + 2] = 0.1f;
				miniMap[index + 3] = 1.0f;
				break;

			default:
				miniMap[index + 0] = 1.0f;
				miniMap[index + 1] = 0.0f;
				miniMap[index + 2] = 0.0f;
				miniMap[index + 3] = 1.0f;
				break;
			}
		}
	}

	Texture* miniMapTexture = new Texture(miniMap, m_mapSize, m_mapSize);
	
	delete miniMap;
	miniMap = nullptr;

	return miniMapTexture;
}

Texture* MapGenerator::GetTexture(type tileType)
{
	switch (tileType)
	{
	case type::Water: return nullptr; break;
	case type::Ground: return m_center0; break;
	case type::TopLeft: return m_topLeft; break;
	case type::TopCenter: return m_topCenter; break;
	case type::TopRight: return m_topRight; break;
	case type::InnerTopLeft: return m_innerTopLeft; break;
	case type::InnerTopRight: return m_innerTopRight; break;
	case type::MiddleLeft: return m_middleLeft; break;
	case type::MiddleRight: return m_middleRight; break;
	case type::InnerBottomLeft: return m_innerBottomLeft; break;
	case type::InnerBottomRight: return m_innerBottomRight; break;
	case type::BottomLeft: return m_bottomLeft; break;
	case type::BottomCenter: return m_bottomCenter; break;
	case type::BottomRight: return m_bottomRight; break;
	default: break;
	}

	return nullptr;
}

void MapGenerator::SetTileSprite(int x, int y)
{
	bool top = false, right = false, bottom = false, left = false, floorAbove = false, floorBelow = false, floorRight = false, floorLeft = false;

	if (x < m_mapSize - 1)
	{
		if (m_cells[x + 1][y].GetPreCellType() == type::Wall)
			right = true;

		if (m_cells[x + 1][y].GetCellType() == type::Ground)
			floorRight = true;
	}

	if (x > 0)
	{
		if (m_cells[x - 1][y].GetPreCellType() == type::Wall)
			left = true;

		if (m_cells[x - 1][y].GetCellType() == type::Ground)
			floorLeft = true;
	}

	if (y < m_mapSize - 1)
	{
		if (m_cells[x][y + 1].GetPreCellType() == type::Wall)
			top = true;

		if (m_cells[x][y + 1].GetCellType() == type::Ground)
			floorAbove = true;
	}

	if (y > 0)
	{
		if (m_cells[x][y - 1].GetPreCellType() == type::Wall)
			bottom = true;

		if (m_cells[x][y - 1].GetCellType() == type::Ground)
			floorBelow = true;
	}

	m_cells[x][y].GetGameObject()->SetTexture(SetWall(top, right, bottom, left, floorAbove, floorBelow, floorRight, floorLeft));
}