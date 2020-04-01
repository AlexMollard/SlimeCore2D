#include "Circle.h"

Circle::Circle(Mesh* mesh) : GameObject(mesh) 
{
	type = ObjectType::Circle;
}

Circle::~Circle()
{
}

void Circle::Update(float deltaTime)
{
	boundingBox.UpdateQuadBoundingBox(position, scale);
	UpdateInteraction(deltaTime);
}
