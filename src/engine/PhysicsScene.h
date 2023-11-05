#pragma once
#include "RigidBody.h"
#include <vector>
#include "glm.hpp"
#include "CollisionManager.h"

#include <functional>
#include <algorithm>

class Renderer2D;

class PhysicsScene
{
public:
	PhysicsScene() = default;
	~PhysicsScene() = default;

	void addActor(RigidBody* actor, std::string name = "", bool isKinematic = false);
	void addActor(const std::vector<RigidBody*>& actors);
	void addActor(RigidBody** actors, int amount);
	void removeActor(RigidBody* actor);

	void update(float dt);

	void setGravity(const glm::vec3 inGravity);
	glm::vec2 getGravity() const;

	void setTimeStep(const float inTimeStep);
	float getTimeStep() const;

protected:
	glm::vec2 m_gravity = glm::vec2(0, 0);
	float m_timeStep = 0.01f;
	std::vector<RigidBody*> m_actors;
	std::vector<RigidBody*> m_dynamicActors;
};
