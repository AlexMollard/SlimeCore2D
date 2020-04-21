#include "Line.h"

Line::Line() : GameObject()
{
	type = ObjectType::Line;
}

Line::~Line()
{
}

void Line::Update(float deltaTime)
{
	boundingBox.UpdateQuadBoundingBox(position, scale);
	//UpdateInteraction(deltaTime);
}

float Line::GetDistance()
{
	return distance;
}

void Line::SetDistance(float newDistance)
{
	distance = newDistance;
}
