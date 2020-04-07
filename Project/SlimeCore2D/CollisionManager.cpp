#include "CollisionManager.h"
#include "Circle.h"
#include "Line.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

glm::vec2 CollisionManager::CircleVsCircle(RigidBody* rbOne, RigidBody* rbTwo)
{
	assert(rbOne && rbTwo && "A Circle was Null in collision check");

	glm::vec2 displacement = (rbOne->GetPos() - rbTwo->GetPos());
	float sumOfRadius = (rbOne->GetScale().x * 0.5f) + (rbTwo->GetScale().x * 0.5f);
	float overLap = (sumOfRadius) - glm::length(displacement);

	if (overLap > 0.0f)
		return glm::normalize(displacement) * overLap;

	return glm::vec2(0);
}

glm::vec2 CollisionManager::CircleVsQuad(RigidBody* rbOne, RigidBody* rbTwo)
{
	return QuadVsCircle(rbTwo, rbOne);
}

glm::vec2 CollisionManager::CircleVsLine(RigidBody* circle, RigidBody* line)
{
	Line* l = dynamic_cast<Line*>(line);

	float position_dot_normal = glm::dot(circle->GetPos(), l->GetNormal());

	float distance = position_dot_normal - (-l->GetDistance() + (circle->GetScale().x * 0.5f));

	if (distance < 0.0f)
		return -(l->GetNormal()) * distance;
	else
		return glm::vec2(0.0f);
}

glm::vec2 CollisionManager::QuadVsCircle(RigidBody* rbOne, RigidBody* rbTwo)
{
	glm::vec2 one_Min = rbOne->GetPos() - (rbOne->GetScale() * 0.5f);
	glm::vec2 one_Max = rbOne->GetPos() + (rbOne->GetScale() * 0.5f);

	glm::vec2 clamped_centre = glm::clamp(rbTwo->GetPos(), one_Min, one_Max);

	glm::vec2 displacement = clamped_centre - rbTwo->GetPos();

	float overlap = (rbTwo->GetScale().x * 0.5f) - glm::length(displacement);

	if (overlap > 0.0f)
	{
		if (rbOne->isKinematic)
			return glm::normalize(-displacement) * overlap;
		
		return glm::normalize(displacement) * overlap;
	}

	return glm::vec2(0.0f);
}

glm::vec2 CollisionManager::QuadVsQuad(RigidBody* rbOne, RigidBody* rbTwo)
{
	glm::vec2 overlapVector = { 0,0 };
	float overlap = 999999999999.0f;

	glm::vec2 one_Min = rbOne->GetPos() - (rbOne->GetScale() * 0.5f);
	glm::vec2 one_Max = rbOne->GetPos() + (rbOne->GetScale() * 0.5f);
	glm::vec2 two_Min = rbTwo->GetPos() - (rbTwo->GetScale() * 0.5f);
	glm::vec2 two_Max = rbTwo->GetPos() + (rbTwo->GetScale() * 0.5f);

	if (one_Max.x > two_Min.x)
	{
		float localOverLap = abs(one_Max.x - two_Min.x);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(-overlap, 0);
		}
	}
	else
		return glm::vec2(0.0f);

	if (two_Max.x > one_Min.x)
	{
		float localOverLap = abs(two_Max.x - one_Min.x);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(overlap, 0);
		}
	}
	else
		return glm::vec2(0.0f);

	if (one_Max.y > two_Min.y)
	{
		float localOverLap = abs(one_Max.y - two_Min.y);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(0, -overlap);
		}
	}
	else
		return glm::vec2(0.0f);

	if (two_Max.y > one_Min.y)
	{
		float localOverLap = abs(two_Max.y - one_Min.y);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(0, overlap);
		}
	}
	else
		return glm::vec2(0.0f);

	return overlapVector;
}

glm::vec2 CollisionManager::QuadVsLine(RigidBody* rbOne, RigidBody* rbTwo)
{	
	Line* l = dynamic_cast<Line*>(rbTwo);
	
	glm::vec2 extents = rbOne->GetScale();
	glm::vec2 extents_neg_x = glm::vec2(-rbOne->GetScale().x, rbOne->GetScale().y);
	
	float dot_extents = glm::dot(extents, l->GetNormal());
	float dot_extents_neg_x = glm::dot(extents_neg_x, l->GetNormal());
	
	float radius = fmaxf(fabsf(dot_extents), fabsf(dot_extents_neg_x));
	
	static Circle projection = Circle(nullptr);
	projection.SetPos(rbOne->GetPos());
	projection.SetMass(rbOne->GetMass());
	projection.SetScale(glm::vec2(radius));

	return CircleVsLine(&projection, rbTwo);
}

glm::vec2 CollisionManager::LineVsCircle(RigidBody* rbOne, RigidBody* rbTwo)
{
	return CircleVsLine(rbTwo, rbOne);
}

glm::vec2 CollisionManager::LineVsQuad(RigidBody* rbOne, RigidBody* rbTwo)
{
	return QuadVsLine(rbTwo,rbOne);
}

glm::vec2 CollisionManager::LineVsLine(RigidBody* rbOne, RigidBody* rbTwo)
{
	return glm::vec2(0);
}
