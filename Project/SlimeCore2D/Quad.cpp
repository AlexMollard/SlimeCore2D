#include "Quad.h"

Quad::Quad(Mesh* mesh) : GameObject(mesh)
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