#include "BoundingBox.h"

void BoundingBox::UpdateQuadBoundingBox(glm::vec2 pos, glm::vec2 size)
{
	m_max = glm::vec2(pos.x + (size.x * 0.5f), pos.y + (size.y * 0.5f));
	m_min = glm::vec2(pos.x - (size.x * 0.5f), pos.y - (size.y * 0.5f));
}

bool BoundingBox::GetIsColliding(const BoundingBox& other) const
{
	bool collisionX = (other.m_min.x > m_max.x || other.m_max.x < m_min.x);

	bool collisionY = (other.m_min.y > m_max.y || other.m_max.y < m_min.y);

	return !(collisionX || collisionY);
}

bool BoundingBox::GetMouseColliding()
{
	glm::vec2 mousePos = m_inputManager->GetMousePos();

	bool collisionX = (mousePos.x > m_min.x&& m_max.x > mousePos.x);

	bool collisionY = (mousePos.y > m_min.y&& m_max.y > mousePos.y);

	return (collisionX && collisionY);
}

glm::vec2& BoundingBox::GetMax()
{
	return m_max;
}

glm::vec2& BoundingBox::GetMin()
{
	return m_min;
}

glm::vec2 BoundingBox::GetPos(glm::vec3 objectPos) const
{
	return objectPos + glm::vec3(m_offset, 0);
}

glm::vec2 BoundingBox::GetScale() const
{
	return m_scale;
}