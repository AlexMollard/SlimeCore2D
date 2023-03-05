#pragma once
#include "Quad.h"
#include <memory>
class Cloud
{
public:
	Cloud(Texture* cloud, Texture* shadow, float size, glm::vec2 startPos);
	~Cloud() = default;

	void Update(float deltaTime);

	float GetSpeed();
	void SetSpeed(float newSpeed);

	std::shared_ptr<GameObject> GetCloud();
	std::shared_ptr<GameObject> GetShadow();
private:
	float speed = 1.0f;

	std::shared_ptr<GameObject> cloudObject = nullptr;
	std::shared_ptr<GameObject> shadowObject = nullptr;
};
