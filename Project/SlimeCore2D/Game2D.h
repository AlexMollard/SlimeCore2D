#pragma once
#include "GameObject.h"
#include "Renderer2D.h"
#include "InputManager.h"

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* renderer = nullptr;
	InputManager* inputManager = nullptr;
	float timer = 0.0f;
};

