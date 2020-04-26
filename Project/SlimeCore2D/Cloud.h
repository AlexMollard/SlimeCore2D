#pragma once
#include "Quad.h"
class Cloud
{
public:
	Cloud(Texture* cloud, Texture* shadow, float size, glm::vec2 startPos);
	~Cloud();

	void Update(float deltaTime);

	float GetSpeed();
	void SetSpeed(float newSpeed);

	GameObject* GetCloud();
	GameObject* GetShadow();
private:
	float speed = 1.0f;

	GameObject* cloudObject = nullptr;
	GameObject* shadowObject = nullptr;
};

