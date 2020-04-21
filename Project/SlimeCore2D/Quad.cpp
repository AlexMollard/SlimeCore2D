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
	boundingBox.UpdateQuadBoundingBox(position, scale);
	UpdateInteraction(deltaTime);
}