
#include "Player.h"

#include "Cell.h"

#include "engine/MemoryDebugging.h"
#include "engine/QuadBatchRenderer.h"
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
	m_cells           = cells;
	m_isMemoryManaged = true;
	camera            = cam;

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

void Player::SetBatch(QuadBatchRenderer* batch)
{
	m_batch = batch;
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

	if (Input::GetKeyPress(SDLK_d) || Input::GetKeyPress(SDLK_a) || Input::GetKeyPress(SDLK_w) || Input::GetKeyPress(SDLK_s))
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
	float speed = (Input::GetKeyPress(SDLK_LSHIFT)) ? 10.0f : 4.0f;

	float moveSpeed = speed * deltaTime;

	glm::vec3 movePos = GetPos();

	if (Input::GetKeyPress(SDLK_w))
		movePos += glm::vec3(0.0f, moveSpeed, 0);

	if (Input::GetKeyPress(SDLK_s))
		movePos += glm::vec3(0.0f, -moveSpeed, 0);

	if (Input::GetKeyPress(SDLK_a))
		movePos += glm::vec3(-moveSpeed, 0.0f, 0);

	if (Input::GetKeyPress(SDLK_d))
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
		const int x = posX + (i % 3) - 1;
		const int y = posY + (i / 3) - 1;
		SetSurroundTile(i, m_cells[x][y].GetGameObject());
	}
}

void Player::SetAllCells(Cell** cells)
{
	m_cells = cells;
}

void Player::PickupItem(Item item)
{
	items.push_back(item);
	OnPickupItem(&items.back());
}

void Player::OnPickupItem(Item* item)
{
	if (item->HasEventHandler(ItemEvent::OnPickup))
	{
		item->handleEvent(ItemEvent::OnPickup, *this);
	}
}

void Player::shootBullet()
{
	glm::vec2 mousePos  = Input::GetMousePos();
	glm::vec3 playerPos = GetPos();

	glm::vec2 direction = glm::normalize(mousePos - glm::vec2(playerPos));

	bullets.push_back({ 1 });

	Bullet& bullet = bullets.back();
	bullet.Create(playerPos, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }, 4575);
	//bullet.UpdatePos();
	m_batch->AddObject(&bullets.back());

	OnShootBullet(&bullets.back());
}

void Player::OnShootBullet(Bullet* bullet)
{
	for (auto& item : items)
	{
		if (item.HasEventHandler(ItemEvent::OnShoot))
		{
			item.handleEvent(ItemEvent::OnShoot, *this);
		}
	};
}

Bullet* Player::GetLastBullet()
{
	return &bullets.back();
}
