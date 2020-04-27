#include "Player.h"

Player::Player() : Quad()
{
}

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
}

void Player::Init(Camera* cam)
{
	isPlayer = true;
	camera = cam;

	player_Idle_Right = new Texture("..\\Textures\\Player\\Knight_Idle_Right.png");
	player_Idle_Left = new Texture("..\\Textures\\Player\\Knight_Idle_Left.png");

	player_Run_Right = new Texture("..\\Textures\\Player\\Knight_Run_Right.png");
	player_Run_Left = new Texture("..\\Textures\\Player\\Knight_Run_Left.png");

	SetBoundingBox(glm::vec2(0, -0.75f), glm::vec2(0.75f, 0.25f));
	SetTexture(player_Idle_Right);
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

	camera->SetPosition(position);
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