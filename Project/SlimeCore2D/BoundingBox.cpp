#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::UpdateBoundingBox(glm::vec3 pos, glm::vec3 size)
{
	topRight = glm::vec2(pos.x + (size.x * 0.5f), pos.y + (size.y * 0.5f));
	bottomLeft = glm::vec2(pos.x - (size.x * 0.5f), pos.y - (size.y * 0.5f));
}

bool BoundingBox::GetIsColliding(glm::vec2 other)
{
	return false;
}

bool BoundingBox::GetIsColliding(float x, float y)
{
	return false;
}

bool BoundingBox::GetMouseColliding()
{
	glm::vec2 mousePos = inputManager->GetMousePos();

	bool collisionX = (mousePos.x > bottomLeft.x&& topRight.x > mousePos.x);

	bool collisionY = (mousePos.y > bottomLeft.y&& topRight.y > mousePos.y);

	return (collisionX && collisionY);
}
