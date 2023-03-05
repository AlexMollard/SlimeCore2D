#include "CollisionManager.h"

#include <algorithm>
#include <numeric>
#include <utility>

CollisionManager::CollisionManager() {}

CollisionManager::~CollisionManager() {}

glm::vec2 CollisionManager::QuadVsQuad(RigidBody* rbOne, RigidBody* rbTwo)
{
	glm::vec2 posOne   = (rbOne->GetUseBoundingBox()) ? rbOne->GetBoundingBox().GetPos(rbOne->GetPos()) : rbOne->GetPos();
	glm::vec2 scaleOne = (rbOne->GetUseBoundingBox()) ? rbOne->GetBoundingBox().GetScale() : rbOne->GetScale();
	glm::vec2 posTwo   = (rbTwo->GetUseBoundingBox()) ? rbTwo->GetBoundingBox().GetPos(rbTwo->GetPos()) : rbTwo->GetPos();
	glm::vec2 scaleTwo = (rbTwo->GetUseBoundingBox()) ? rbTwo->GetBoundingBox().GetScale() : rbTwo->GetScale();

	glm::vec2 one_Min = posOne - (scaleOne * 0.5f);
	glm::vec2 one_Max = posOne + (scaleOne * 0.5f);
	glm::vec2 two_Min = posTwo - (scaleTwo * 0.5f);
	glm::vec2 two_Max = posTwo + (scaleTwo * 0.5f);

	float overlap_x = std::clamp(std::min(one_Max.x, two_Max.x) - std::max(one_Min.x, two_Min.x), 0.0f, std::numeric_limits<float>::infinity());
	float overlap_y = std::clamp(std::min(one_Max.y, two_Max.y) - std::max(one_Min.y, two_Min.y), 0.0f, std::numeric_limits<float>::infinity());

	if (overlap_x < overlap_y)
		return (one_Max.x - two_Min.x < two_Max.x - one_Min.x) ? glm::vec2(-overlap_x, 0.0f) : glm::vec2(overlap_x, 0.0f);

	return (one_Max.y - two_Min.y < two_Max.y - one_Min.y) ? glm::vec2(0.0f, -overlap_y) : glm::vec2(0.0f, overlap_y);
}
