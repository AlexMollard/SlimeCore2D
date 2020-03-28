#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::UpdateBoundingBox(glm::vec2 pos, glm::vec2 size)
{
	topRight = glm::vec2(pos.x + (size.x * 0.5f), pos.y + (size.y * 0.5f));
	bottomLeft = glm::vec2(pos.x - (size.x * 0.5f), pos.y - (size.y * 0.5f));
}

bool BoundingBox::GetIsColliding(BoundingBox& other)
{
	bool collisionX = (other.bottomLeft.x > topRight.x || other.topRight.x < bottomLeft.x);

	bool collisionY = (other.bottomLeft.y > topRight.y || other.topRight.y < bottomLeft.y);

	return !(collisionX || collisionY);
}

bool BoundingBox::GetMouseColliding()
{
	glm::vec2 mousePos = inputManager->GetMousePos();

	bool collisionX = (mousePos.x > bottomLeft.x&& topRight.x > mousePos.x);

	bool collisionY = (mousePos.y > bottomLeft.y&& topRight.y > mousePos.y);

	return (collisionX && collisionY);
}
