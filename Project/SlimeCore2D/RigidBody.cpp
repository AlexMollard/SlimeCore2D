#include "pch.h"
#include "RigidBody.h"

#include <iostream>
#include "Cell.h"

glm::vec2 RigidBody::GetVelocity() const
{
	return m_velocity;
}

void RigidBody::SetPos(glm::vec3 newPos)
{
	SetLocalPosition(newPos);
	if (m_parent == nullptr)
		SetPosition(GetLocalPosition());
	else
		SetPosition(GetLocalPosition() + m_parent->GetPos());

	for (auto child : m_children)
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
	m_velocity = newVel;
}

void RigidBody::SetMass(float newMass)
{
	m_mass = newMass;
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
	m_velocity += force / m_mass;
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
	return m_scale;
}

void RigidBody::SetScale(const glm::vec2& newScale)
{
	m_scale = newScale;
	m_model = glm::scale(m_model, glm::vec3(GetScale(), 1.0f));
}

glm::vec3 RigidBody::GetLocalPosition() const
{
	return m_localPosition;
}

void RigidBody::SetLocalPosition(glm::vec3 val)
{
	m_localPosition = val;
}

glm::vec3 RigidBody::GetPosition() const
{
	return m_position;
}

void RigidBody::SetPosition(glm::vec3 val)
{
	m_position = val;
}

int RigidBody::GetID() const
{
	return m_id;
}

void RigidBody::SetID(int val)
{
	m_id = val;
}

bool RigidBody::GetUseBoundingBox() const
{
	return m_useBoundingBox;
}

void RigidBody::SetUseBoundingBox(bool val)
{
	m_useBoundingBox = val;
}

RigidBody* RigidBody::GetParent() const
{
	return m_parent;
}

void RigidBody::SetParent(RigidBody* newParent)
{
	newParent->AddChild(this);
	m_parent = newParent;
}

RigidBody* RigidBody::GetChild(int index) const
{
	if (index < 0 || index >= m_children.size())
		return nullptr;
	return m_children[index];
}

void RigidBody::AddChild(RigidBody* newChild)
{
	m_children.push_back(newChild);
}

void RigidBody::UpdatePos()
{
	SetPosition(GetLocalPosition() + ((m_parent == nullptr) ? glm::vec3(0.0f) : m_parent->GetPos()));
	for (auto child : m_children)
		child->UpdatePos();
}

const RigidBody* RigidBody::GetSurroundTile(int index) const
{
	return m_surroundingTiles.at(index);
}

void RigidBody::SetSurroundTile(int index, RigidBody* cell)
{
	m_surroundingTiles[index] = cell;
}

float RigidBody::GetRotation() const
{
	return m_rotation;
}

bool RigidBody::GetIsColliding(const RigidBody& other) const
{
	return GetBoundingBox().GetIsColliding(other.GetBoundingBox());
}

void RigidBody::SetNormal(glm::vec2 newNormal)
{
	m_normal = glm::normalize(newNormal);
}

void RigidBody::SetRotation(float val)
{
	m_rotation = val;
}

glm::vec2 RigidBody::GetNormal() const
{
	return m_normal;
}

const std::array<RigidBody*, 9>& RigidBody::GetSurroundingTiles() const
{
	return m_surroundingTiles;
}

glm::mat4 RigidBody::GetModel() const
{
	return m_model;
}

void RigidBody::SetModel(glm::mat4 newModel)
{
	m_model = newModel;
}

void RigidBody::AddVelocity(glm::vec2 newVel)
{
	m_velocity += newVel;
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (GetKinematic())
		return;
	ApplyForce(gravity * timeStep);
	SetPosition(GetPosition() + glm::vec3(m_velocity, 0) * timeStep);
	SetPos(GetPosition());
}

bool RigidBody::GetKinematic() const
{
	return m_isKinematic;
}

void RigidBody::SetKinematic(bool value)
{
	m_isKinematic = value;
}

void RigidBody::ApplyDrag(float timeStep)
{
	glm::vec2 darg = { 0.0f, 0.0f };

	darg.x = (GetVelocity().x > 0.0f) ? -m_drag : m_drag;
	darg.y = (GetVelocity().y > 0.0f) ? -m_drag : m_drag;

	ApplyForce(darg * timeStep);
}

float RigidBody::GetMass() const
{
	return m_mass;
}

const BoundingBox& RigidBody::GetBoundingBox() const
{
	return m_boundingBox;
}

void RigidBody::SetBoundingBox(const glm::vec2& offset, const glm::vec2& scale)
{
	m_boundingBox.m_offset = offset;
	m_boundingBox.m_scale  = scale;
	SetUseBoundingBox(true);
}

void RigidBody::ResolveCollision(RigidBody* other)
{
	if (GetNormal() == glm::vec2(0.0f))
		return;
	SetNormal(glm::normalize(GetNormal()));
	glm::vec2 relativeVelocity = other->GetVelocity() - m_velocity;
	float elasticity           = 1.0f;
	float j                    = -(1.0f + elasticity) * glm::dot(relativeVelocity, GetNormal()) / (1.0f / m_mass + 1.0f / other->m_mass);
	glm::vec2 force            = GetNormal() * j;
	ApplyForceToActor(other, force);
}
