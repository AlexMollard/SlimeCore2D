#include "RigidBody.h"

#include <iostream>
#include "Cell.h"

glm::vec2 RigidBody::GetVelocity() const
{
	return velocity;
}

void RigidBody::SetPos(glm::vec3 newPos)
{
	SetLocalPosition(newPos);
	if (parent == nullptr)
		SetPosition(GetLocalPosition());
	else
		SetPosition(GetLocalPosition() + parent->GetPos());

	for (auto child : children)
		child->UpdatePos();
}

void RigidBody::SetPos(float x, float y, float z)
{
	SetPos(glm::vec3(x, y, z));
}

glm::vec3 RigidBody::GetPos() const
{
	return GetPosition();
}

void RigidBody::SetVelocity(glm::vec2 newVel)
{
	velocity = newVel;
}

void RigidBody::SetMass(float newMass)
{
	mass = newMass;
}

void RigidBody::ApplyForceToActor(RigidBody* obj, glm::vec2 force)
{
	if (GetKinematic())
		return;
	ApplyForce(-force);
	if (!obj->GetKinematic())
		obj->ApplyForce(force);
}

void RigidBody::ApplyForce(glm::vec2 force)
{
	velocity += force / mass;
}

void RigidBody::ApplyOffSetToActor(RigidBody* obj, glm::vec3 overlap)
{
	if (this->GetKinematic() || obj->GetKinematic())
	{
		if (this->GetKinematic())
			obj->SetPos(obj->GetPos() + overlap);
		else
			this->SetPos(this->GetPos() - overlap);
	}
	else
	{
		this->SetPos(GetPos() - overlap * 0.5f);
		obj->SetPos(obj->GetPos() + overlap * 0.5f);
	}
}

glm::vec2 RigidBody::GetScale() const
{
	return scale;
}

void RigidBody::SetScale(const glm::vec2& newScale)
{
	scale = newScale;
	model = glm::scale(model, glm::vec3(GetScale(), 1.0f));
}

glm::vec3 RigidBody::GetLocalPosition() const
{
	return localPosition;
}

void RigidBody::SetLocalPosition(glm::vec3 val)
{
	localPosition = val;
}

glm::vec3 RigidBody::GetPosition() const
{
	return position;
}

void RigidBody::SetPosition(glm::vec3 val)
{
	position = val;
}

int RigidBody::GetID() const
{
	return ID;
}

void RigidBody::SetID(int val)
{
	ID = val;
}

bool RigidBody::GetUseBoundingBox() const
{
	return useBoundingBox;
}

void RigidBody::SetUseBoundingBox(bool val)
{
	useBoundingBox = val;
}

RigidBody* RigidBody::GetParent() const
{
	return parent;
}

void RigidBody::SetParent(RigidBody* newParent)
{
	newParent->AddChild(this);
	parent = newParent;
}

RigidBody* RigidBody::GetChild(int index) const
{
	if (index < 0 || index >= children.size())
		return nullptr;
	return children[index];
}

void RigidBody::AddChild(RigidBody* newChild)
{
	children.push_back(newChild);
}

void RigidBody::UpdatePos()
{
	SetPosition(GetLocalPosition() + ((parent == nullptr) ? glm::vec3(0.0f) : parent->GetPos()));
	for (auto child : children)
		child->UpdatePos();
}

const RigidBody* RigidBody::GetSurroundTile(int index) const
{
	return surroundingTiles.at(index);
}

void RigidBody::SetSurroundTile(int index, RigidBody* cell)
{
	surroundingTiles[index] = cell;
}

float RigidBody::GetRotation() const
{
	return rotation;
}

bool RigidBody::GetIsColliding(const RigidBody& other) const
{
	return GetBoundingBox().GetIsColliding(other.GetBoundingBox());
}

void RigidBody::SetNormal(glm::vec2 newNormal)
{
	normal = glm::normalize(newNormal);
}

void RigidBody::SetRotation(float val)
{
	rotation = val;
}

glm::vec2 RigidBody::GetNormal() const
{
	return normal;
}

const std::array<RigidBody*, 9>& RigidBody::GetSurroundingTiles() const
{
	return surroundingTiles;
}

glm::mat4 RigidBody::GetModel() const
{
	return model;
}

void RigidBody::SetModel(glm::mat4 newModel)
{
	model = newModel;
}

void RigidBody::AddVelocity(glm::vec2 newVel)
{
	velocity += newVel;
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (GetKinematic())
		return;
	ApplyForce(gravity * timeStep);
	SetPosition(GetPosition() + glm::vec3(velocity, 0) * timeStep);
	SetPos(GetPosition());
}

bool RigidBody::GetKinematic() const
{
	return isKinematic;
}

void RigidBody::SetKinematic(bool value)
{
	isKinematic = value;
}

void RigidBody::ApplyDrag(float timeStep)
{
	glm::vec2 darg = { 0.0f, 0.0f };

	darg.x = (GetVelocity().x > 0.0f) ? -drag : drag;
	darg.y = (GetVelocity().y > 0.0f) ? -drag : drag;

	ApplyForce(darg * timeStep);
}

float RigidBody::GetMass() const
{
	return mass;
}

const BoundingBox& RigidBody::GetBoundingBox() const
{
	return boundingBox;
}

void RigidBody::SetBoundingBox(const glm::vec2& offset, const glm::vec2& scale)
{
	boundingBox.offset = offset;
	boundingBox.scale  = scale;
	SetUseBoundingBox(true);
}

void RigidBody::ResolveCollision(RigidBody* other)
{
	if (GetNormal() == glm::vec2(0.0f))
		return;
	SetNormal(glm::normalize(GetNormal()));
	glm::vec2 relativeVelocity = other->GetVelocity() - velocity;
	float elasticity           = 1.0f;
	float j                    = -(1.0f + elasticity) * glm::dot(relativeVelocity, GetNormal()) / (1.0f / mass + 1.0f / other->mass);
	glm::vec2 force            = GetNormal() * j;
	ApplyForceToActor(other, force);
}
