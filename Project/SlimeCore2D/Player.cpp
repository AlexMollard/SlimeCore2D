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
	type = ObjectType::Quad;
	camera = cam;

	player_Idle_Right = new Texture("..\\Textures\\Knight_Idle_Right.png");
	player_Idle_Left = new Texture("..\\Textures\\Knight_Idle_Left.png");

	player_Run_Right = new Texture("..\\Textures\\Knight_Run_Right.png");
	player_Run_Left = new Texture("..\\Textures\\Knight_Run_Left.png");
}

void Player::Update(float deltaTime)
{
	camera->SetPosition(position);
	
	playerMovement(deltaTime);

	static bool right = true;

	if (InputManager::GetKeyPress(Keycode::A))
	{
		right = false;
		SetTexture(player_Run_Left);
	}

	if (InputManager::GetKeyPress(Keycode::D))
	{
		right = true;
		SetTexture(player_Run_Right);
	}

	if (InputManager::GetKeyPress(Keycode::D) || InputManager::GetKeyPress(Keycode::A) || InputManager::GetKeyPress(Keycode::W) || InputManager::GetKeyPress(Keycode::S))
	{
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
}

void Player::playerMovement(float deltaTime)
{
	float speed = (InputManager::GetKeyPress(Keycode::LEFT_SHIFT)) ? 10.0f : 4.0f;

	float moveSpeed = speed * deltaTime;

	if (InputManager::GetKeyPress(Keycode::W))
		position += glm::vec2(0.0f, moveSpeed);

	if (InputManager::GetKeyPress(Keycode::S))
		position += glm::vec2(0.0f, -moveSpeed);

	if (InputManager::GetKeyPress(Keycode::A))
		position += glm::vec2(-moveSpeed, 0.0f);

	if (InputManager::GetKeyPress(Keycode::D))
		position += glm::vec2(moveSpeed, 0.0f);

	SetPos(position);
}
