#include "PhysicsScene.h"
#include <iostream>
#include <sstream>

PhysicsScene::PhysicsScene() : timeStep(0.01f), gravity(glm::vec2(0, -10.0f))
{
	tex = new TextRenderer();
}

void PhysicsScene::addActor(RigidBody* actor, std::string name, bool isKinematic)
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i] == actor)
		{
			std::cout << "Actor already exist in physics scene" << std::endl;
			break;
		}
	}
	actor->name = name;
	actor->SetKinematic(isKinematic);
	actors.push_back(actor);
}

void PhysicsScene::addActor(std::vector<RigidBody*> actors)
{
	addActor(actors.data(), actors.size());
}

void PhysicsScene::addActor(RigidBody** actors, int amount)
{
	for (int i = 0; i < amount; i++)
		this->actors.push_back(actors[i]);
}

void PhysicsScene::removeActor(RigidBody* actor)
{
	auto location = std::find(actors.begin(), actors.end(), actor);
	actors.erase(location);
}

void PhysicsScene::update(float dt) {

	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;
	while (accumulatedTime >= timeStep)
	{
		for (auto pActor : actors)
		{
			pActor->fixedUpdate(gravity, timeStep);
		}
		accumulatedTime -= timeStep;
	}

	
	for (int i = 0; i < actors.size(); i++)//)auto object : actors)
	{
		RigidBody* object = actors[i];

		for (int y = i + 1; y < actors.size(); y++)
		{
			RigidBody* other = actors[y];
			
			if (other->GetKinematic() && object->GetKinematic())
				continue;
			
			if (std::find(object->collided.begin(), object->collided.end(), other) != object->collided.end())
				continue;

			if (object->GetIsColliding(other))
			{
				glm::vec2 overLap = GetOverLap(*other->GetBoundingBox(), *object->GetBoundingBox());

				other->SetNormal(overLap);

				other->ApplyOffSetToActor(object, overLap);
				other->resolveCollision(object);

				//Debug::PrintLog(object->name + " Collided with: " + other->name);

				other->collided.push_back(object);
			}
		}
	}
	for (int i = 0;  i < actors.size(); i++)
	{
		actors[i]->collided.clear();
	}
}

void PhysicsScene::Debug()
{
	for (int i = 0; i < actors.size(); i++)
	{
		std::ostringstream ss;
		ss << actors[i]->GetVelocity().x;
		std::string x(ss.str());

		ss << actors[i]->GetVelocity().y;
		std::string y(ss.str());

		std::string text = std::string(actors[i]->name + " Velocity: (" + x + ", " + y + ")");
		tex->RenderText(text, 50, 20 * i + 50, 1, glm::vec3(1));
	}
}

glm::vec2 PhysicsScene::GetOverLap(BoundingBox& one, BoundingBox& two)
{
	glm::vec2 overlapVector = { 0,0 };
	float overlap = 999999999999.0f;

	if (one.GetMax().x > two.GetMin().x)
	{
		float localOverLap = abs(one.GetMax().x - two.GetMin().x);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(-overlap, 0);
		}
	}
	else
		return glm::vec2(0.0f);

	if (two.GetMax().x > one.GetMin().x)
	{
		float localOverLap = abs(two.GetMax().x - one.GetMin().x);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(overlap, 0);
		}
	}
	else
		return glm::vec2(0.0f);

	if (one.GetMax().y > two.GetMin().y)
	{
		float localOverLap = abs(one.GetMax().y - two.GetMin().y);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(0, -overlap);
		}
	}
	else
		return glm::vec2(0.0f);

	if (two.GetMax().y > one.GetMin().y)
	{
		float localOverLap = abs(two.GetMax().y - one.GetMin().y);
		if (localOverLap < overlap)
		{
			overlap = localOverLap;
			overlapVector = glm::vec2(0, overlap);
		}
	}
	else
		return glm::vec2(0.0f);

	return overlapVector;
}