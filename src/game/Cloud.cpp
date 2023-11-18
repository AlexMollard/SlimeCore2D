#include "Cloud.h"
#include "engine/MemoryDebugging.h"

Cloud::Cloud(Texture* cloud, Texture* shadow, float size, glm::vec2 startPos)
{
	speed = (rand() % 10) * 0.1f + 1;

	m_cloudObject = std::make_shared<GameObject>();
	m_cloudObject->SetPos({ startPos, -0.75f });
	m_cloudObject->SetTexture(cloud);
	m_cloudObject->SetSpriteWidth(32);
	m_cloudObject->SetScale(glm::vec2(2 * size, 1 * size));

	m_shadowObject = std::make_shared<GameObject>();
	m_shadowObject->SetPos(0, -4.0f, 0.01);
	m_shadowObject->SetTexture(shadow);
	m_shadowObject->SetSpriteWidth(32);
	m_shadowObject->SetScale(glm::vec2(2 * size, 1 * size));

	m_shadowObject->SetParent(m_cloudObject.get());
}

void Cloud::Update(float deltaTime)
{
	glm::vec3 position = m_cloudObject->GetPos();

	if (position.x > 75)
	{
		position = glm::vec3(-75, rand() % 150 - 75, -0.75f);
		speed = (rand() % 3) + 1;
	}

	m_cloudObject->SetPos(position.x + (deltaTime * speed), position.y, position.z);
}

float Cloud::GetSpeed()
{
	return speed;
}

void Cloud::SetSpeed(float newSpeed)
{
	speed = newSpeed;
}

std::shared_ptr<GameObject> Cloud::GetCloud()
{
	return m_cloudObject;
}

std::shared_ptr<GameObject> Cloud::GetShadow()
{
	return m_shadowObject;
}