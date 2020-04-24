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
	 
	SetBoundingBox(glm::vec2(0, -0.75f),glm::vec2(0.75f, 0.25f));
}

void Player::Update(float deltaTime)
{
	
	playerMovement(deltaTime);

	static bool right = true;

	if (InputManager::GetKeyPress(Keycode::A))
		right = false;

	if (InputManager::GetKeyPress(Keycode::D))
		right = true;

	if (InputManager::GetKeyPress(Keycode::D) || InputManager::GetKeyPress(Keycode::A) || InputManager::GetKeyPress(Keycode::W) || InputManager::GetKeyPress(Keycode::S))
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
	float speed = (InputManager::GetKeyPress(Keycode::LEFT_SHIFT)) ? 10.0f : 4.0f;

	float moveSpeed = speed * deltaTime;

	glm::vec3 movePos = GetPos();

	if (InputManager::GetKeyPress(Keycode::W))
		movePos += glm::vec3(0.0f, moveSpeed, 0);

	if (InputManager::GetKeyPress(Keycode::S))
		movePos += glm::vec3(0.0f, -moveSpeed, 0);

	if (InputManager::GetKeyPress(Keycode::A))
		movePos += glm::vec3(-moveSpeed, 0.0f, 0);

	if (InputManager::GetKeyPress(Keycode::D))
		movePos += glm::vec3(moveSpeed, 0.0f, 0);

	SetPos(movePos);
}
