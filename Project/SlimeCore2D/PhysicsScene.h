#pragma once
#include "RigidBody.h"
#include <vector>
#include "glm.hpp"
#include "TextRenderer.h"
#include "CollisionManager.h"

#include <functional>
#include <algorithm>
using Collision_Function = std::function<glm::vec2(RigidBody * rbOne, RigidBody * rbTwo)>;

class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene() { delete tex; };

	void addActor(RigidBody* actor, std::string name, bool isKinematic = false);
	void addActor(std::vector<RigidBody*> actors);
	void addActor(RigidBody** actors, int amount);
	void removeActor(RigidBody* actor);

	void update(float dt);
	void Debug();

	void setGravity(const glm::vec3 gravity) { this->gravity = gravity; }
	glm::vec2 getGravity() const { return gravity; }

	void setTimeStep(const float timeStep) { this->timeStep = timeStep; }
	float getTimeStep() const { return timeStep; }
	
	static Collision_Function collisionFunctions[];

protected:
	TextRenderer* tex = nullptr;
	glm::vec2 gravity;
	float timeStep = 0.01f;
	std::vector<RigidBody*> actors;
};
