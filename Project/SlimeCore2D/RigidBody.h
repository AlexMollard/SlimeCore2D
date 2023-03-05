#pragma once
#include "BoundingBox.h"
#include "glm.hpp"
#include <array>
#include <string>
#include <vector>

class RigidBody
{
public:
	std::string name;

	glm::vec2 GetVelocity() const;

	void SetPos(glm::vec3 newPos);
	void SetPos(float x, float y, float z);
	glm::vec3 GetPos() const;

	void SetVelocity(glm::vec2 newVel);
	void AddVelocity(glm::vec2 newVel);

	void fixedUpdate(glm::vec2 gravity, float timeStep);

	bool GetKinematic() const;
	void SetKinematic(bool value);

	void ApplyDrag(float timeStep);

	float GetMass() const;
	void SetMass(float newMass);

	void ApplyForceToActor(RigidBody* obj, glm::vec2 force);
	void ApplyForce(glm::vec2 force);

	void ApplyOffSetToActor(RigidBody* obj, glm::vec3 overlap);
	void ResolveCollision(RigidBody* other);

	bool GetIsColliding(const RigidBody& other) const;

	virtual void SetNormal(glm::vec2 newNormal);
	glm::vec2 GetNormal() const;

	glm::mat4 GetModel() const;
	void SetModel(glm::mat4 newModel);

	const BoundingBox& GetBoundingBox() const;
	void SetBoundingBox(const glm::vec2& offset, const glm::vec2& scale);

	glm::vec2 GetScale() const;
	void SetScale(const glm::vec2& newScale);

	RigidBody* GetParent() const;
	void SetParent(RigidBody* newParent);

	RigidBody* GetChild(int index) const;
	void AddChild(RigidBody* newChild);

	void UpdatePos();

	const RigidBody* GetSurroundTile(int index) const;
	void SetSurroundTile(int index, RigidBody* cell);
	const std::array<RigidBody*, 9>& GetSurroundingTiles() const;


	float GetRotation() const;
	void SetRotation(float val);

	glm::vec3 GetLocalPosition() const;
	void SetLocalPosition(glm::vec3 val);

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 val);

	int GetID() const;
	void SetID(int val);

	bool GetUseBoundingBox() const;
	void SetUseBoundingBox(bool val);

private:
	bool isKinematic    = false;
	int ID              = -404;
	bool useBoundingBox = false;

	glm::vec3 position      = glm::vec3(0.0f);
	glm::vec3 localPosition = glm::vec3(0.0f);
	glm::vec2 scale         = glm::vec2(1.0f);
	glm::vec2 normal        = glm::vec2(1.0f);

	BoundingBox boundingBox;

	RigidBody* parent = nullptr;
	std::vector<RigidBody*> children;

	std::array<RigidBody*, 9> surroundingTiles;

	glm::vec2 velocity = glm::vec2(0.0f);
	glm::mat4 model    = glm::mat4(1.0f);
	float rotation     = 0.0f;

	float drag = 5.0f;
	float mass = 1.0f;
};
