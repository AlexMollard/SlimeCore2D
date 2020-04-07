#pragma once
#include "RigidBody.h"

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	static glm::vec2 CircleVsCircle(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 CircleVsQuad(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 CircleVsLine(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 QuadVsCircle(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 QuadVsQuad(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 QuadVsLine(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 LineVsCircle(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 LineVsQuad(RigidBody* rbOne, RigidBody* rbTwo);
	static glm::vec2 LineVsLine(RigidBody* rbOne, RigidBody* rbTwo);

private:
};

