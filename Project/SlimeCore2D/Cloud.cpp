#include "Cloud.h"

Cloud::Cloud(Texture* cloud, Texture* shadow, float size, glm::vec2 startPos)
{
	speed = (rand() % 10) * 0.1f + 1;

	cloudObject = new GameObject();
	cloudObject->SetPos({ startPos, -0.95f });
	cloudObject->SetTexture(cloud);
	cloudObject->SetSpriteWidth(32);
	cloudObject->SetScale(glm::vec2(2 * size, 1 * size));

	shadowObject = new GameObject();
	shadowObject->SetPos(0, -4.0f, 0.01);
	shadowObject->SetTexture(shadow);
	shadowObject->SetSpriteWidth(32);
	shadowObject->SetScale(glm::vec2(2 * size, 1 * size));

	shadowObject->SetParent(cloudObject);
}

Cloud::~Cloud()
{
	delete cloudObject;
	cloudObject = nullptr;

	delete shadowObject;
	shadowObject = nullptr;
}

void Cloud::Update(float deltaTime)
{
	glm::vec3 position = cloudObject->GetPos();

	if (position.x > 75)
	{
		position = glm::vec3(-75, rand() % 100 - 50, -0.95f);
		speed = (rand() % 3) + 1;
	}

	cloudObject->SetPos(position.x + (deltaTime * speed), position.y, position.z);
}

float Cloud::GetSpeed()
{
	return speed;
}

void Cloud::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

GameObject* Cloud::GetCloud()
{
	return cloudObject;
}

GameObject* Cloud::GetShadow()
{
	return shadowObject;
}