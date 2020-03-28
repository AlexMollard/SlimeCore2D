#pragma once
#include "glm.hpp"
#include <string>
#include "BoundingBox.h"

class RigidBody 
{
public:
	std::string name = "";

	glm::vec2 GetVelocity() { return velocity; };

	void SetPos(glm::vec2 newPos);
	void SetPos(float x, float y);
	glm::vec2 GetPos();

	void SetVelocity(glm::vec2 newVel) { velocity = newVel; };
	void AddVelocity(glm::vec2 newVel) { velocity += newVel; };

	void fixedUpdate(glm::vec2 gravity, float timeStep);
	void Update(float deltaTime);

	bool GetKinematic() { return isKinematic; };
	void SetKinematic(bool value) { isKinematic = value; };

	float GetMass() { return mass; };
	void SetMass(float newMass) { mass = newMass; };

	void ApplyForceToActor(RigidBody* obj, glm::vec2 force);
	void ApplyForce(glm::vec2 force);

	void ApplyOffSetToActor(RigidBody* obj, glm::vec2 overlap);

	bool GetIsColliding(RigidBody* other);

	BoundingBox* GetBoundingBox();

protected:
	bool isKinematic = false;

	BoundingBox boundingBox;

	InputManager* inputManager = InputManager::GetInstance();

	glm::vec2 position = glm::vec2(0);
	glm::vec2 velocity = glm::vec2(0);
	glm::mat4 model = glm::mat4(1);

	float mass = 1.0f;
};