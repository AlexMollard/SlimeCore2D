#pragma once
#include "RigidBody.h"

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	static glm::vec2 CircleVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 CircleVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 CircleVsLine(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 QuadVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 QuadVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 QuadVsLine(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 LineVsCircle(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 LineVsQuad(const RigidBody* rbOne, const RigidBody* rbTwo);
	static glm::vec2 LineVsLine(const RigidBody* rbOne, const RigidBody* rbTwo);

private:
};

