#include "Player.h"

Player::Player() : Quad() {}

Player::~Player()
{
	delete player_Idle_Right;
	player_Idle_Right = nullptr;

	delete player_Idle_Left;
	player_Idle_Left = nullptr;

	delete player_Run_Right;
	player_Run_Right = nullptr;

	delete player_Run_Left;
	player_Run_Left = nullptr;

	delete player_Shadow;
	player_Shadow = nullptr;
}

void Player::Init(Camera* cam, Cell** cells, GameObject* shadow)
{
	this->cells = cells;
	isPlayer    = true;
	camera      = cam;

	player_Idle_Right = new Texture("..\\Textures\\Player\\Knight_Idle_Right.png");
	player_Idle_Left  = new Texture("..\\Textures\\Player\\Knight_Idle_Left.png");

	player_Run_Right = new Texture("..\\Textures\\Player\\Knight_Run_Right.png");
	player_Run_Left  = new Texture("..\\Textures\\Player\\Knight_Run_Left.png");

	player_Shadow = new Texture("..\\Textures\\Player\\player_shadow.png");

	SetBoundingBox(glm::vec2(0, -0.75f), glm::vec2(1, 0.5f));
	SetTexture(player_Idle_Right);

	shadow->SetTexture(player_Shadow);
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
			SetTexture(player_Run_Right);
		else
			SetTexture(player_Run_Left);
		SetFrameRate(0.1f);
	}
	else
	{
		if (right)
			SetTexture(player_Idle_Right);
		else
			SetTexture(player_Idle_Left);

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
		SetSurroundTile(i, cells[x][y].object);
	}
}

void Player::SetAllCells(Cell** cells)
{
	this->cells = cells;
}
