#include "Quad.h"

Quad::Quad() : GameObject()
{
	type = ObjectType::Quad;
}

Quad::~Quad()
{
}

void Quad::Update(float deltaTime)
{
	//boundingBox.UpdateQuadBoundingBox(position, scale);
	//UpdateInteraction(deltaTime);

	if (hasAnimation)
	{
		frameRateTimer += deltaTime;

		if (frameRateTimer > frameRate)
		{
			frameRateTimer = 0;
			AdvanceFrame();
		}
	}
}