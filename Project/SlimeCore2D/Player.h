#pragma once
#include "Quad.h"
#include "Camera.h"
class Player : public Quad
{
public:
	Player();
	~Player();

	void Init(Camera* cam);

	void Update(float deltaTime);
	void playerMovement(float deltaTime);
private:
	Camera* camera = nullptr;

	Texture* player_Idle_Left = nullptr;
	Texture* player_Idle_Right = nullptr;
	Texture* player_Run_Left = nullptr;
	Texture* player_Run_Right = nullptr;
};
