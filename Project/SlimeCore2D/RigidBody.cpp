#include "RigidBody.h"


void RigidBody::SetPos(glm::vec2 newPos)
{
	SetPos(newPos.x, newPos.y);
}

void RigidBody::SetPos(float x, float y)
{
	position.x = x;
	position.y = y;

	model[3] = glm::vec4(position,0, 1);
}

glm::vec2 RigidBody::GetPos()
{
	return position;
}

void RigidBody::ApplyForceToActor(RigidBody* obj, glm::vec2 force)
{
	this->ApplyForce(force);
	obj->ApplyForce(-force);
}

void RigidBody::ApplyForce(glm::vec2 force)
{
	velocity += force / mass;
}

void RigidBody::ApplyOffSetToActor(RigidBody* obj, glm::vec2 overlap)
{
	if (overlap.x < 0.001f || overlap.y < 0.001f)
		return;

	if (GetKinematic() != true)
		this->SetPos(GetPos() + overlap * 0.5f);

	if (obj->GetKinematic() == true)
		return;

	obj->SetPos(obj->GetPos() - overlap * 0.5f);
}

bool RigidBody::GetIsColliding(RigidBody* other)
{
	return (GetBoundingBox()->GetIsColliding(*other->GetBoundingBox()));
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (GetKinematic())
	{
		SetVelocity(glm::vec2(0));
		return;
	}

	ApplyForce(gravity * timeStep);
	position += velocity * timeStep;
	SetPos(position);
}

BoundingBox* RigidBody::GetBoundingBox()
{
	return &boundingBox;
}