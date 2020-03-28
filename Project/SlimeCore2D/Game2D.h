#pragma once
#include "Button.h"
#include "Renderer2D.h"

class Game2D
{
public:
	Game2D();
	~Game2D();

	void Update(float deltaTime);
	void Draw();

private:
	Renderer2D* renderer = Renderer2D::GetInstance();
	InputManager* inputManager = InputManager::GetInstance();

	float timer = 0.0f;
};

