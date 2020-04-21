#include "Circle.h"

Circle::Circle() : GameObject() 
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
