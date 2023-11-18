
#include "Player.h"
#include "engine/MemoryDebugging.h"

#include "Cell.h"
#include "engine/ResourceManager.h"

Player::~Player()
{
	delete m_playerIdleRight;
	m_playerIdleRight = nullptr;

	delete m_playerIdleLeft;
	m_playerIdleLeft = nullptr;

	delete m_playerRunright;
	m_playerRunright = nullptr;

	delete m_playerRunLeft;
	m_playerRunLeft = nullptr;

	delete m_playerShadow;
	m_playerShadow = nullptr;
}

void Player::Init(Camera* cam, Cell** cells, GameObject* shadow)
{
	m_cells = cells;
	m_isMemoryManaged = true;
	camera = cam;

	m_playerIdleRight = new Texture(ResourceManager::GetTexturePath("Player/Knight_Idle_Right"));
	m_playerIdleLeft  = new Texture(ResourceManager::GetTexturePath("Player/Knight_Idle_Left"));

	m_playerRunright = new Texture(ResourceManager::GetTexturePath("Player/Knight_Run_Right"));
	m_playerRunLeft  = new Texture(ResourceManager::GetTexturePath("Player/Knight_Run_Left"));

	m_playerShadow = new Texture(ResourceManager::GetTexturePath("Player/player_shadow"));

	SetBoundingBox(glm::vec2(0, -0.75f), glm::vec2(1, 0.5f));
	SetTexture(m_playerIdleRight);

	shadow->SetTexture(m_playerShadow);
	shadow->SetParent(this);
	shadow->UpdatePos();
	shadow->SetScale(glm::vec2(1, 2));
}

void Player::Update(float deltaTime)
{
	glm::vec2 mousePos = Input::GetMousePos();

	playerMovement(deltaTime);

	static bool right = true;

	if (mousePos.x < 0)
		right = false;

	if (mousePos.x >= 0)
		right = true;

	if (Input::GetKeyPress(Keycode::D) || Input::GetKeyPress(Keycode::A) || Input::GetKeyPress(Keycode::W) || Input::GetKeyPress(Keycode::S))
	{
		if (right)
			SetTexture(m_playerRunright);
		else
			SetTexture(m_playerRunLeft);
		SetFrameRate(0.1f);
	}
	else
	{
		if (right)
			SetTexture(m_playerIdleRight);
		else
			SetTexture(m_playerIdleLeft);

		SetFrameRate(0.15f);
	}

	UpdateSpriteTimer(deltaTime);
	UpdateSurroundingTiles();
}

void Player::playerMovement(float deltaTime)
{
	float speed = (Input::GetKeyPress(Keycode::LEFT_SHIFT)) ? 10.0f : 4.0f;

	float moveSpeed = speed * deltaTime;

	glm::vec3 movePos = GetPos();

	if (Input::GetKeyPress(Keycode::W))
		movePos += glm::vec3(0.0f, moveSpeed, 0);

	if (Input::GetKeyPress(Keycode::S))
		movePos += glm::vec3(0.0f, -moveSpeed, 0);

	if (Input::GetKeyPress(Keycode::A))
		movePos += glm::vec3(-moveSpeed, 0.0f, 0);

	if (Input::GetKeyPress(Keycode::D))
		movePos += glm::vec3(moveSpeed, 0.0f, 0);

	SetPos(movePos);
}

void Player::UpdateSurroundingTiles()
{
	const int MAPSIZE = 100 * 0.5f;
	const int posX    = (int)GetPos().x + MAPSIZE;
	const int posY    = (int)GetPos().y + MAPSIZE - 1;

	for (int i = 0; i < 9; ++i)
	{
		const int x         = posX + (i % 3) - 1;
		const int y         = posY + (i / 3) - 1;
		SetSurroundTile(i, m_cells[x][y].GetGameObject());
	}
}

void Player::SetAllCells(Cell** cells)
{
	m_cells = cells;
}
