#include "CollisionManager.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

glm::vec2 CollisionManager::CircleVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	assert(rbOne && rbTwo && "A Circle was Null in collision check");

	glm::vec2 displacement = (rbOne->position - rbTwo->position);
	float sumOfRadius = (rbOne->scale.x * 0.5f) + (rbTwo->scale.x * 0.5f);
	float overLap = (sumOfRadius) - glm::length(displacement);

	if (overLap > 0.0f)
		return glm::normalize(displacement) * overLap;

	return glm::vec2(0);
}

glm::vec2 CollisionManager::CircleVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return QuadVsCircle(rbTwo, rbOne);
}

glm::vec2 CollisionManager::CircleVsLine(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return glm::vec2();
}

glm::vec2 CollisionManager::QuadVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	glm::vec2 one_Min = rbOne->position - (rbOne->scale * 0.5f);
	glm::vec2 one_Max = rbOne->position + (rbOne->scale * 0.5f);

	glm::vec2 clamped_centre = glm::clamp(rbTwo->position, one_Min, one_Max);

	glm::vec2 displacement = clamped_centre - rbTwo->position;

	float overlap = (rbTwo->scale.x * 0.5f) - glm::length(displacement);

	if (overlap > 0.0f)
	{
		if (rbOne->isKinematic)
			return glm::normalize(-displacement) * overlap;
		
		return glm::normalize(displacement) * overlap;
	}

	return glm::vec2(0.0f);
}

glm::vec2 CollisionManager::QuadVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	glm::vec2 overlapVector = { 0,0 };
	float overlap = 999999999999.0f;

	glm::vec2 one_Min = rbOne->position - (rbOne->scale * 0.5f);
	glm::vec2 one_Max = rbOne->position + (rbOne->scale * 0.5f);
	glm::vec2 two_Min = rbTwo->position - (rbTwo->scale * 0.5f);
	glm::vec2 two_Max = rbTwo->position + (rbTwo->scale * 0.5f);

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

glm::vec2 CollisionManager::QuadVsLine(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return glm::vec2();
}

glm::vec2 CollisionManager::LineVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return glm::vec2();
}

glm::vec2 CollisionManager::LineVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return glm::vec2();
}

glm::vec2 CollisionManager::LineVsLine(const RigidBody* rbOne, const RigidBody* rbTwo)
{
	return glm::vec2();
}
