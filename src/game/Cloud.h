#pragma once
#include "engine/GameObject.h"
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

	std::shared_ptr<GameObject> m_cloudObject = nullptr;
	std::shared_ptr<GameObject> m_shadowObject = nullptr;
};
